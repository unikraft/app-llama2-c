// SPDX-License-Identifier: GPL-2.0-only
/*
 * L2E OS Driver
 * Derived from eprog_kern by Richard Weinberger
 * Copyright (C) 2023 Richard Weinberger <richard@nod.at>
 * Copyright (C) 2023 Vulcan Ignis <1ohm@pm.me>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/freezer.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
#include <linux/umh.h>
#include <linux/usermode_driver.h>

// Max 1024 chars
static char *quest = "Sudo make me a";
module_param(quest, charp, S_IRUGO);

extern char embedded_umh_start;
extern char embedded_umh_end;

static struct umd_info l2e_ctx = {
	.driver_name = "l2e",
};

static struct task_struct *l2e_thread_tsk;
static char *read_buf;
static char *write_buf;
#define READ_BUFSZ 4096
#define WRITE_BUFSZ 4096


static int l2e_thread(void *data)
{
	struct umd_info *l2e_ctx = data;
	loff_t pos = 0;
	ssize_t nread;
	ssize_t nwrite;

	set_freezable();

	pr_alert(">> GURU UNMEDITATION :: L2E :: LLAMA HAS AWAKENED <<");
	pr_alert("l2e.quest: %s\n", quest);
	for (;;) {
		if (kthread_should_stop())
			break;

		if (try_to_freeze())
			continue;

		nread = kernel_read(l2e_ctx->pipe_from_umh, read_buf, READ_BUFSZ - 1, &pos);
		if (nread > 0) {
			read_buf[nread] = '\0';
			pr_alert("%s", read_buf);
			write_buf=quest;
			nwrite = kernel_write(l2e_ctx->pipe_to_umh, write_buf, WRITE_BUFSZ , &pos);

		} else if (nread == -ERESTARTSYS) {
			break;
		} else {
			pr_err("Fatal error while reading from userspace: %ld\n", nread);
			/*
			 * Suspend ourself and wait for termination.
			 */
			set_current_state(TASK_INTERRUPTIBLE);
			schedule();
		}
	}

	return 0;
}

static void kill_umh(struct umd_info *l2e_ctx)
{
	struct pid *l2e_tgid = l2e_ctx->tgid;

	kill_pid(l2e_tgid, SIGKILL, 1);
	wait_event(l2e_tgid->wait_pidfd, thread_group_exited(l2e_tgid));
	umd_cleanup_helper(l2e_ctx);
}

static int __init l2e_init(void)
{
	int ret;

	read_buf = kmalloc(READ_BUFSZ, GFP_KERNEL);
	if (!read_buf) {
		ret = -ENOMEM;
		goto out;
	}

	ret = umd_load_blob(&l2e_ctx, &embedded_umh_start, &embedded_umh_end - &embedded_umh_start);
	if (ret) {
		pr_err("Unable to load embedded l2e blob: %i\n", ret);
		kfree(read_buf);
		goto out;
	}

	ret = fork_usermode_driver(&l2e_ctx);
	if (ret) {
		pr_err("Unable to start embedded l2e: %i\n", ret);
		umd_unload_blob(&l2e_ctx);
		kfree(read_buf);
		goto out;
	}

	l2e_thread_tsk = kthread_create(l2e_thread, &l2e_ctx, "l2e_thread");
	if (IS_ERR(l2e_thread_tsk)) {
		ret = PTR_ERR(l2e_thread_tsk);
		pr_err("Unable to start kernel thread: %i\n", ret);
		kill_umh(&l2e_ctx);
		umd_unload_blob(&l2e_ctx);
		kfree(read_buf);
		goto out;
	}

	wake_up_process(l2e_thread_tsk);
	ret = 0;
out:
	return ret;
}

static void __exit l2e_exit(void)
{
	kthread_stop(l2e_thread_tsk);
	kill_umh(&l2e_ctx);
	kfree(read_buf);
	umd_unload_blob(&l2e_ctx);
}

module_init(l2e_init);
module_exit(l2e_exit);

MODULE_AUTHOR("Richard Weinberger <richard@nod.at>");
MODULE_AUTHOR("Vulcan Ignis <1ohm@pm.me>");
MODULE_DESCRIPTION("L2E OS Driver");
MODULE_LICENSE("GPL");
