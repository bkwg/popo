#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>

#include "tracer.h"
#include "tracee.h"
#include "disas.h"
#include "bk_types.h"

/* ====================================================================== */
/* PRIVATE                                                                */
/* ====================================================================== */
 
/* ====================================================================== */
/* PTRACE FUNCTIONS                                                       */
/* ====================================================================== */
 
static BkStatus popo_attach_target(pid_t pid)
{
    if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1)
    {
        perror("[!] ptrace ATTACH");
        return BK_STATUS_FAILURE;
    }

    return BK_STATUS_SUCCESS;
}

static BkStatus popo_get_registers(pid_t pid, struct user_regs_struct* regs)
{
    // TODO: check that appropriately (cf man RETURN VALUE)
    if (ptrace(PTRACE_GETREGS, pid, 0, regs) == -1)
    {
        perror("ptrace GETREGS");
        return BK_STATUS_FAILURE;
    }

    return BK_STATUS_SUCCESS;
}

static BkStatus popo_get_instruction(pid_t pid, struct user_regs_struct* regs, U64* insn)
{
    if ((*insn = ptrace(PTRACE_PEEKTEXT, pid, regs->rip, 0)) == -1)
    {
        perror("ptrace PEEKTEXT");
        return BK_STATUS_FAILURE;
    }

    return BK_STATUS_SUCCESS;
}

//static BkStatus popo_single_step_tracee(pid_t tracee_pid)
//{
//    if (ptrace(PTRACE_SINGLESTEP, tracee_pid, 0, 0) == -1)
//    {
//        perror("ptrace SINGLESTEP");
//        return BK_STATUS_FAILURE;
//    }
//    return BK_STATUS_SUCCESS;
//}

static BkStatus popo_cont_tracee(pid_t tracee_pid)
{
    if (ptrace(PTRACE_CONT, tracee_pid, 0, 0) == -1)
    {
        perror("ptrace CONT");
        return BK_STATUS_FAILURE;
    }
    return BK_STATUS_SUCCESS;
}

/* ====================================================================== */
/* ACTION PROCEDURES                                                      */
/* ====================================================================== */
 
static U64 g_insn_counter = 0;

static void increment_insn_count(void)
{
    ++g_insn_counter;
}

static BkStatus popo_do(pid_t tracee_pid)
{
    // TODO: actions performed shall be subjected to user choices

    struct user_regs_struct regs = { 0 };
    BkStatus s = BK_STATUS_FAILURE;
    
    //// 1. Get regs
    s = popo_get_registers(tracee_pid, &regs);
    if (BK_FAILURE(s)) return BK_STATUS_FAILURE;
    //
    // 2. Get insn
    U64 insn = 0;
    s = popo_get_instruction(tracee_pid, &regs, &insn);
    if (BK_FAILURE(s)) return BK_STATUS_FAILURE;

    //printf("--- %lx\n", insn);

    // 4. Increment
    increment_insn_count();

    //regs->rdi = 77;
    //if ((insn = ptrace(PTRACE_SETREGS, pid, 0, regs)) == -1)
    //{
    //    perror("ptrace PEEKTEXT");
    //    return BK_STATUS_FAILURE;
    //}

    return BK_STATUS_SUCCESS;
}

/* ====================================================================== */
/* SIGNAL HANDLERS                                                        */
/* ====================================================================== */

static const char* signal_handler_msg_map[] = {
    "NONE",
    "[+] Tracee '%d' exited with status %d\n",
    "[+] Tracee '%d' killed by signal %d\n",
    "[+] Tracee '%d' dumped by signal %d\n",
    "[+] Tracee '%d' stopped by signal %d\n",
    "[+] Tracee '%d' continued by signal %d\n",
    "[+] Something else happened to the tracee (pid: %d, signal: %d)\n"
};

static BkStatus popo_handle_non_trap_signal(pid_t tracee_pid, siginfo_t* infop)
{
    // TODO: this check may be useless
    U64 last_element = BK_ARRAY_LENGTH(signal_handler_msg_map) - 1;
    U8 slot = infop->si_code < last_element ? infop->si_code : last_element;
    printf(
        signal_handler_msg_map[slot],
        infop->si_pid,
        infop->si_status);

    return BK_STATUS_SUCCESS;
}

static BkStatus popo_handle_trap(pid_t tracee_pid)
{
    // TODO: actions performed shall be subjected to user choices
   
    //ACTIONS : here we perform actions the instructions
    BkStatus s = BK_STATUS_FAILURE;
    s = popo_do(tracee_pid);

    if (BK_FAILURE(s)) return BK_STATUS_FAILURE;

    return BK_STATUS_SUCCESS;
}

static BkStatus popo_handle_tracee_signal(pid_t tracee_pid, siginfo_t *infop)
{
    //TODO: differenciate between x86 and x86_64
    switch (infop->si_code)
    {
        // TODO: for now we just deal with TRAP
        case CLD_TRAPPED:
            popo_handle_trap(tracee_pid);
            break;
        default:
            popo_handle_non_trap_signal(tracee_pid, infop);
    }

    return BK_STATUS_SUCCESS;
}

/* ====================================================================== */
/* WAIT & SIGNAL FUNCTIONS                                                */
/* ====================================================================== */

static BkStatus popo_wait_for_signal(pid_t tracee_pid, siginfo_t* infop)
{
    if (waitid(P_PID, tracee_pid, infop, WSTOPPED | WCONTINUED | WEXITED) != 0)
    {
        perror("waitid");
        return BK_STATUS_FAILURE;
    }

    return BK_STATUS_SUCCESS;
}

static BkBool is_sigint_or_sigquit(siginfo_t* infop)
{
    if ((infop->si_code == CLD_TRAPPED)
        && !((infop->si_status == SIGINT) || (infop->si_status == SIGQUIT)))
        return BK_FALSE;
    return BK_TRUE;
}

static BkBool is_killed_exited_or_dumped(siginfo_t* infop)
{
    if ((infop->si_code != CLD_KILLED)
        && (infop->si_code != CLD_EXITED)
        && (infop->si_code != CLD_DUMPED))
            return BK_FALSE;
    return BK_TRUE;
}

/* ====================================================================== */

/**
 * See man 7 signal for a detailed list of signals
 */
static BkStatus popo_trace_tracee(pid_t tracee_pid)
{
    siginfo_t infop = { 0 };

    printf("[+] Starting tracing tracee\n");

    BkStatus s = BK_STATUS_FAILURE;

    while ((s = BK_SUCCESS(popo_wait_for_signal(tracee_pid, &infop)))
       && !is_killed_exited_or_dumped(&infop)
       && !is_sigint_or_sigquit(&infop))
    {
        s = popo_handle_tracee_signal(tracee_pid, &infop);
        if (BK_FAILURE(s)) return BK_STATUS_FAILURE;

        // TODO: number of steps shall be subejcted to user choices
        //       maybe generalize this function and a flag that
        //       specify the steps i.e single steps, bb, branch, ret, etc...
        //s = popo_single_step_tracee(tracee_pid);
        s = popo_cont_tracee(tracee_pid);
        if (BK_FAILURE(s)) return BK_STATUS_FAILURE;
    }

    if (BK_FAILURE(s)) return BK_STATUS_FAILURE;

    popo_handle_non_trap_signal(tracee_pid, &infop);

    return BK_STATUS_SUCCESS;
}

/* ====================================================================== */
/* PUBLIC                                                                 */
/* ====================================================================== */
 
BkStatus popo_spawn(PopoTracee* tracee, const char* image_path)
{
    printf("[+] Spawning a process from image '%s'\n", image_path);
    pid_t child;
    
    child = fork();

    if (child == -1)
    {
        perror("fork");
        return BK_STATUS_FAILURE;
    }
    else if (child == 0)
    {
        puts("fiston");
        return tracee->run(image_path);
    }
    else if (child > 0)
    {
        puts("daron");
        tracee->pid = child;
        tracee->image_path = image_path;
        return BK_STATUS_SUCCESS;
    }

    return BK_STATUS_FAILURE;
}

BkStatus popo_attach(PopoTracee* tracee, pid_t pid)
{
    printf("[+] Attaching to process '%d'\n", pid);
    BkStatus s = popo_attach_target(pid);
    if (BK_FAILURE(s)) return BK_STATUS_FAILURE;

    tracee->pid = pid;

    return BK_STATUS_SUCCESS;
}

BkStatus popo_launch(PopoArgs args)
{
    PopoTracee tracee = { 0 };
    popo_init_tracee(&tracee);

    BkStatus s = BK_STATUS_FAILURE;

    if (args.image_path != 0)
        s = popo_spawn(&tracee, args.image_path);
    else if (args.pid != 0)
        s = popo_attach(&tracee, args.pid);

    if (BK_FAILURE(s)) return BK_STATUS_FAILURE;

    printf("[+] Starting tracing process '%d'\n", tracee.pid);
    s = popo_trace_tracee(tracee.pid);
    if (BK_FAILURE(s)) return BK_STATUS_FAILURE;

    printf("[RESULT] Instructions executed: %lu\n", g_insn_counter);

    return BK_STATUS_SUCCESS;
}
