#include "command.h"
#include "vtysh_config.h"
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>

DEFUN(config_ping,
       config_ping_cmd,
       "ping WORD",
       "send echo messages\n"
       "Ping destination address or hostname\n")
{
  cmd_execute_system_command ("ping", 1, argv);
  return CMD_SUCCESS;
}

DEFUN(config_ping_count,
       config_ping_cmd_count,
       "ping WORD NUM",
       "send count echo messages\n"
       "Ping destination address or hostname\n"
	   "Stop after sending NUM ECHO_REQUEST packets\n")
{
	char *myargv[5];

	if(!isdigit(argv[1][0]))
	{
		vty_out(vty, "Invalid number '%s', Please input the count number\n", argv[1]);
		return CMD_ERR_NOTHING_TODO;
	}
	myargv[0] = argv[0];
	myargv[1] = "-c";
	myargv[2] = argv[1];
	cmd_execute_system_command ("ping", 3, myargv);
	return CMD_SUCCESS;
}

DEFUN(config_traceroute,
       config_traceroute_cmd,
       "traceroute WORD",
       "Trace route to destination\n"
       "Trace route to destination address or hostname\n")
{
  cmd_execute_system_command ("traceroute", 1, argv);
  return CMD_SUCCESS;
}

DEFUN(config_telnet,
       config_telnet_cmd,
       "telnet WORD",
       "Open a telnet connection\n"
       "IP address or hostname of a remote system\n")
{
  cmd_execute_system_command ("telnet", 1, argv);
  return CMD_SUCCESS;
}

DEFUN(config_telnet_port,
       config_telnet_port_cmd,
       "telnet WORD PORT",
       "Open a telnet connection\n"
       "IP address or hostname of a remote system\n"
       "TCP Port number\n")
{
  cmd_execute_system_command ("telnet", 2, argv);
  return CMD_SUCCESS;
}

DEFUN(config_reboot,
       config_reboot_cmd,
       "reboot",
       "reboot the system\n"
       "reboot the system\n")
{
  vty_out(vty, "System will reboot, please waiting...\n");
  cmd_execute_system_command ("reboot", 0, argv);
  return CMD_SUCCESS;
}

DEFUN(config_poweroff,
       config_poweroff_cmd,
       "poweroff",
       "power off the system\n"
       "power off the system\n")
{
  vty_out(vty, "System power off!\n");
  cmd_execute_system_command ("poweroff", 0, argv);
  return CMD_SUCCESS;
}

DEFUN(config_tftp,
       config_tftp_cmd,
       "tftp (get|put) WORD WORD A.B.C.D",
       "begin a tftp session\n"
       "get file from remote\n"
	   "put file to remote\n"
	   "local file name\n"
	   "remote file name\n"
	   "tftp server address\n")
{
	char *myargv[10];
	char localfile[256];

	if(strchr(argv[1], '/'))
	{
		vty_out(vty, "invalid local file name.please remove the '/' character.\n");
		return CMD_ERR_NOTHING_TODO;
	}
	sprintf(localfile, "%s/%s", CONFIG_DIR, argv[1]);
	if(strcmp(argv[0], "put") == 0)
		myargv[0] = "-p";
	else
		myargv[0] = "-g";
	myargv[1] = "-l";
	myargv[2] = localfile;
	myargv[3] = "-r";
	myargv[4] = argv[2];
	myargv[5] = argv[3];
	return cmd_execute_system_command("tftp", 6, myargv);;
}

DEFUN (service_telnet_server,
       service_telnet_server_cmd,
       "service telnet-server",
       "Set up telnet daemon service\n"
       "Enable telnet daemon\n")
{
	config_del_line(config_top, "service telnet-server");
	config_add_line(config_top, "service telnet-server");

	ENSURE_CONFIG(vty);

	return cmd_execute_system_command("telnetd", 0, NULL);
}

DEFUN (no_service_telnet_server,
       no_service_telnet_server_cmd,
       "no service telnet-server",
       NO_STR
       "Disable telnet server service\n"
       "Disable telnet server\n")
{
	char *myargv[2];

	config_del_line(config_top, "service telnet-server");	
	myargv[0] = "telnetd";
	return cmd_execute_system_command("killall", 1, myargv);
}

DEFUN (service_ssh_server,
       service_ssh_server_cmd,
       "service ssh-server",
       "Set up ssh daemon service\n"
       "Enable ssh daemon\n")
{
	config_del_line(config_top, "service ssh-server");
	config_add_line(config_top, "service ssh-server");
	ENSURE_CONFIG(vty);
	return cmd_execute_system_command("dropbear", 0, NULL);
}

DEFUN (no_service_ssh_server,
       no_service_ssh_server_cmd,
       "no service ssh-server",
       NO_STR
       "Disable ssh server service\n"
       "Disable ssh server\n")
{
	char *myargv[2];

	config_del_line(config_top, "service ssh-server");	
	myargv[0] = "dropbear";
	return cmd_execute_system_command("killall", 1, myargv);
}

#include <sys/vfs.h>    /* or <sys/statfs.h> */

DEFUN(config_dir,
       config_dir_cmd,
       "dir",
       "list the flash files\n")
{
	char *myargv[10];
	struct statfs buf;

	myargv[0] = "-Alh";
	myargv[1] = CONFIG_DIR;
	cmd_execute_system_command ("ls", 2, myargv);

	memset(&buf, 0, sizeof(buf));
	if(statfs(CONFIG_DIR, &buf) < 0)
	{
		myargv[0] = "-h";
		myargv[1] = CONFIG_DIR;
		return cmd_execute_system_command ("df", 2, myargv);
	}
	vty_out(vty, "\nTotal size:%dM free:%dM\n", 
		(buf.f_bsize >> 10) * (buf.f_blocks >> 10), 
		(buf.f_bsize >> 10) * (buf.f_bavail >> 10)
	);
	return 0;
}

#include <sys/vfs.h>    /* or <sys/statfs.h> */

DEFUN(config_sysinfo,
       config_sysinfo_cmd,
       "sysinfo",
       "display the system info\n")
{
	system("top -n 1 | head -n 5");
}

DEFUN(config_delete,
       config_delete_cmd,
       "delete WORD",
       "delete the flash files\n"
	   "the file need delete\n")
{
	char *myargv[10];
	char filename[1024];
	char linkname[1024];

	if(strcmp(argv[0], "ASM-bak.bin") == 0)
	{
		vty_out(vty, "The backup image is not allowed to delete\n");
		return CMD_ERR_NOTHING_TODO;
	}
	memset(linkname, 0, sizeof(linkname));
	sprintf(filename, "%s/ASM.bin", CONFIG_DIR);
	readlink(filename, linkname, sizeof(linkname));
	if((strcmp(argv[0], "ASM.bin") == 0) || (strcmp(linkname, argv[0]) == 0))
	{
		vty_out(vty, "The boot image is not allowed to delete\n");
		return CMD_ERR_NOTHING_TODO;
	}
	
	sprintf(filename, "%s/%s", CONFIG_DIR, argv[0]);
	myargv[0] = "-f";
	myargv[1] = filename;
	return cmd_execute_system_command ("rm", 2, myargv);
}

DEFUN (config_tcpdump,
       config_tcpdump_cmd,
       "tcpdump INTERFACE EXPRESSION",
       "tcpdump packet \n"
       "the interface name\n"
	   "the expression, use '-' replace ' '. eg. tcp-and-port-80")
{
	char *myargv[10];
	int i;

	for(i = 0; i < strlen(argv[1]); i ++)
	{
		if(argv[1][i] == '-')
			argv[1][i] = ' ';
	}

	myargv[0] = "-i";
	myargv[1] = argv[0];
	myargv[2] = "-n";
	myargv[3] = "-Z";
	myargv[4] = "admin";
	myargv[5] = argv[1];

	return cmd_execute_system_command ("tcpdump", 6, myargv);
}

DEFUN(config_logging,
       config_logging_cmd,
       "logging (memory|WORD)",
       "config the logging\n"
	   "logging to the memory only\n"
	   "loggint to the memory and host.the ip address or ip:port,eg 192.168.1.1:514")
{
	char *myargv[10];
	
	config_del_line_byleft(config_top, "logging");

	if(strcmp(argv[0], "memory") == 0)
	{
		config_add_line(config_top, "logging memory");
		ENSURE_CONFIG(vty);
		
		myargv[0] = "syslogd";
		cmd_execute_system_command("killall", 1, myargv);
		return cmd_execute_system_command("syslogd", 0, myargv);
	}
	else
	{
		config_add_line(config_top, "logging %s", argv[0]);
		ENSURE_CONFIG(vty);

		myargv[0] = "syslogd";
		cmd_execute_system_command("killall", 1, myargv);
		myargv[0] = "-L";
		myargv[1] = "-R";
		myargv[2] = argv[0];
		return cmd_execute_system_command("syslogd", 3, myargv);
	}
}

DEFUN(config_no_logging,
       config_no_logging_cmd,
       "no logging",
	   NO_STR
       "Disable the logging\n")
{	
	char *myargv[10];
	config_del_line_byleft(config_top, "logging");
	myargv[0] = "syslogd";
	return cmd_execute_system_command("killall", 1, myargv);
}

DEFUN(config_date,
       config_date_cmd,
       "date WORD WORD",
       "Set the date\n"
	   "the date YYYY-MM-DD\n"
	   "the time HH:MM:SS"
	   )
{	
	char *myargv[10];
	char date[256];

	myargv[0] = "-s";
	sprintf(date, "%s %s", argv[0], argv[1]);
	myargv[1] = date;
	cmd_execute_system_command("date", 2, myargv);
	myargv[0] = "-s";
	return cmd_execute_system_command("hwclock", 1, myargv);
}

/* Show date. */
DEFUN (show_date,
       show_date_cmd,
       "show date",
       SHOW_STR
       "Displays the current date\n")
{
	return cmd_execute_system_command("date", 0, argv);
}

/* Show version. */
DEFUN (show_logging,
       show_logging_cmd,
       "show logging",
       SHOW_STR
       "Displays the logging information\n")
{
	char *myargv[10];

	if(access("/var/log/messages", R_OK) != 0)
	{
		vty_out(vty, "Please first enable the logging\n");
		return CMD_ERR_NOTHING_TODO;
	}
	myargv[0] = "-n";
	myargv[1] = "1000";
	myargv[2] = "/var/log/messages";
	return cmd_execute_system_command("tail", 3, myargv);
}



int cmd_tool_init()
{
  /* Each node's basic commands. */
  cmd_install_element (VIEW_NODE, &config_ping_cmd);
  cmd_install_element (VIEW_NODE, &config_ping_cmd_count);
  cmd_install_element (VIEW_NODE, &config_traceroute_cmd);
  cmd_install_element (VIEW_NODE, &config_telnet_cmd);
  cmd_install_element (VIEW_NODE, &config_telnet_port_cmd);

  cmd_install_element (ENABLE_NODE, &config_ping_cmd);
  cmd_install_element (ENABLE_NODE, &config_ping_cmd_count);
  cmd_install_element (ENABLE_NODE, &config_traceroute_cmd);
  cmd_install_element (ENABLE_NODE, &config_telnet_cmd);
  cmd_install_element (ENABLE_NODE, &config_telnet_port_cmd);
  cmd_install_element (ENABLE_NODE, &config_reboot_cmd);
  cmd_install_element (ENABLE_NODE, &config_poweroff_cmd);
  cmd_install_element (ENABLE_NODE, &config_tftp_cmd);
  cmd_install_element (ENABLE_NODE, &config_dir_cmd);
  cmd_install_element (ENABLE_NODE, &config_delete_cmd);
  cmd_install_element (ENABLE_NODE, &config_tcpdump_cmd);
  cmd_install_element (ENABLE_NODE, &show_logging_cmd);
  cmd_install_element (ENABLE_NODE, &config_sysinfo_cmd);
  cmd_install_element (ENABLE_NODE, &show_date_cmd);

  cmd_install_element (CONFIG_NODE, &config_ping_cmd);
  cmd_install_element (CONFIG_NODE, &config_ping_cmd_count);
  cmd_install_element (CONFIG_NODE, &config_traceroute_cmd);
  cmd_install_element (CONFIG_NODE, &config_telnet_cmd);
  cmd_install_element (CONFIG_NODE, &config_telnet_port_cmd);
  cmd_install_element (CONFIG_NODE, &service_telnet_server_cmd);
  cmd_install_element (CONFIG_NODE, &no_service_telnet_server_cmd);
  cmd_install_element (CONFIG_NODE, &service_ssh_server_cmd);
  cmd_install_element (CONFIG_NODE, &no_service_ssh_server_cmd);
  cmd_install_element (CONFIG_NODE, &config_tftp_cmd);
  cmd_install_element (CONFIG_NODE, &config_dir_cmd);
  cmd_install_element (CONFIG_NODE, &config_logging_cmd);
  cmd_install_element (CONFIG_NODE, &config_no_logging_cmd);
  cmd_install_element (CONFIG_NODE, &show_logging_cmd);
  cmd_install_element (CONFIG_NODE, &config_date_cmd);
  cmd_install_element (CONFIG_NODE, &show_date_cmd);

  return 0;
}
