#include "command.h"
#include "vtysh_config.h"
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>

/* Configration from terminal */
DEFUN (config_terminal,
       config_terminal_cmd,
       "configure terminal",
       "Configuration from vty interface\n"
       "Configuration terminal\n")
{
  vty->node = CONFIG_NODE;
  return CMD_SUCCESS;
}

/* Enable command */
DEFUN (enable, 
       config_enable_cmd,
       "enable",
       "Turn on privileged mode command\n")
{
  /* If enable password is NULL, change to ENABLE_NODE */
  if ((host.enable == NULL && host.enable_encrypt == NULL) ||
      vty->type == VTY_FILE)
    vty->node = ENABLE_NODE;
  else
    vty->node = AUTH_ENABLE_NODE;

  return CMD_SUCCESS;
}

/* Disable command */
DEFUN (disable, 
       config_disable_cmd,
       "disable",
       "Turn off privileged mode command\n")
{
  if (vty->node == ENABLE_NODE)
    vty->node = VIEW_NODE;
  return CMD_SUCCESS;
}

/* Down vty node level. */
DEFUN (config_exit,
       config_exit_cmd,
       "exit",
       "Exit current mode and down to previous mode\n")
{
  switch (vty->node)
    {
    case VIEW_NODE:
    case ENABLE_NODE:
		exit (0);
		break;
    case CONFIG_NODE:
      vty->node = ENABLE_NODE;
      break;
    default:
      break;
    }
  return CMD_SUCCESS;
}
       
/* End of configuration. */
DEFUN (config_end,
       config_end_cmd,
       "end",
       "End current mode and change to enable mode.")
{
  switch (vty->node)
    {
    case VIEW_NODE:
    case ENABLE_NODE:
      /* Nothing to do. */
      break;
    case CONFIG_NODE:
      vty->node = ENABLE_NODE;
      break;
    default:
      break;
    }
  return CMD_SUCCESS;
}

/* Show version. */
DEFUN (show_version,
       show_version_cmd,
       "show version",
       SHOW_STR
       "Displays the version information\n")
{
	char info[1024];

	FILE *fp;
	fp = fopen("/BUILD", "r");
	if(fp)
	{
		fgets(info, sizeof(info), fp);
		fclose(fp);
	}
	else
	{
		sprintf(info, "Build on:%s %s", __DATE__, __TIME__);
	}
	vty_out (vty, "%s\n", info);
	
	fp = fopen("/COPYRIGHT", "r");
	if(fp)
	{
		fgets(info, sizeof(info), fp);
		fclose(fp);
	}
	else
	{
		sprintf(info, "Copyright 1996-2010, EXAMPLE Tech Co., Ltd.");
	}
	vty_out (vty, "%s\n", info);

	return CMD_SUCCESS;
}


/* Help display function for all node. */
DEFUN (config_list,
       config_list_cmd,
       "list",
       "Print command list\n")
{
  int i;
  struct cmd_node *cnode = vector_slot (cmdvec, vty->node);
  struct cmd_element *cmd;

  for (i = 0; i < vector_max (cnode->cmd_vector); i++)
    if ((cmd = vector_slot (cnode->cmd_vector, i)) != NULL)
      vty_out (vty, "  %s%s", cmd->string,
	       VTY_NEWLINE);
  return CMD_SUCCESS;
}

/* Write current configuration into file. */
DEFUN (config_write_file, 
       config_write_file_cmd,
       "write file",  
       "Write running configuration to memory, network, or terminal\n"
       "Write to configuration file\n")
{
	FILE *fp;

	if (host.config == NULL)
	{
		vty_out (vty, "Can't save to configuration file, using vtysh.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	fp = fopen(host.config, "w");
	if(fp == NULL)
	{
      vty_out (vty, "Can't open configuration file %s.%s", host.config, VTY_NEWLINE);
      return CMD_WARNING;
    }
  
	config_dump(fp);
	fflush(fp);
	vty_out (vty, "Configuration saved to %s%s", host.config, VTY_NEWLINE);
	return CMD_SUCCESS;
}

ALIAS (config_write_file, 
       config_write_cmd,
       "write",  
       "Write running configuration to memory, network, or terminal\n");

ALIAS (config_write_file, 
       config_write_memory_cmd,
       "write memory",  
       "Write running configuration to memory, network, or terminal\n"
       "Write configuration to the file (same as write file)\n");



/* Write current configuration into the terminal. */
DEFUN (config_write_terminal,
       config_write_terminal_cmd,
       "write terminal",
       "Write running configuration to memory, network, or terminal\n"
       "Write to terminal\n")
{
  config_dump(stderr);
  return CMD_SUCCESS;
}

/* Write current configuration into the terminal. */
ALIAS (config_write_terminal,
       show_running_config_cmd,
       "show running-config",
       SHOW_STR
       "running configuration\n");

/* Write startup configuration into the terminal. */
DEFUN (show_startup_config,
       show_startup_config_cmd,
       "show startup-config",
       SHOW_STR
       "Contentes of startup configuration\n")
{
  char buf[BUFSIZ];
  FILE *confp;

  confp = fopen (host.config, "r");
  if (confp == NULL)
    {
      vty_out (vty, "Can't open configuration file [%s]%s",
	       host.config, VTY_NEWLINE);
      return CMD_WARNING;
    }

  while (fgets (buf, BUFSIZ, confp))
    {
      char *cp = buf;

      while (*cp != '\r' && *cp != '\n' && *cp != '\0')
	cp++;
      *cp = '\0';

      vty_out (vty, "%s%s", buf, VTY_NEWLINE);
    }

  fclose (confp);

  return CMD_SUCCESS;
}

/* Hostname configuration */
DEFUN (config_hostname, 
       hostname_cmd,
       "hostname WORD",
       "Set system's network name\n"
       "This system's network name\n")
{
	if (!isalpha((int) *argv[0]))
	{
	  vty_out (vty, "Please specify string starting with alphabet%s", VTY_NEWLINE);
	  return CMD_WARNING;
	}

	if (host.name)
	XFREE (0, host.name);

	host.name = strdup (argv[0]);
	sethostname(host.name, strlen(host.name));
	config_del_line_byleft(config_top, "hostname");
	config_add_line(config_top, "hostname %s", host.name);
	return CMD_SUCCESS;
}

DEFUN (config_no_hostname, 
       no_hostname_cmd,
       "no hostname [HOSTNAME]",
       NO_STR
       "Reset system's network name\n"
       "Host name of this router\n")
{
  if (host.name)
    XFREE (0, host.name);
  host.name = NULL;
  config_del_line_byleft(config_top, "hostname ");
  return CMD_SUCCESS;
}

static unsigned char itoa64[] =	
"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void
to64(char *s, long v, int n)
{
  while (--n >= 0) 
    {
      *s++ = itoa64[v&0x3f];
      v >>= 6;
    }
}

static char *zencrypt (char *passwd)
{
  char salt[6];
  struct timeval tv;
  char *crypt (const char *, const char *);

  gettimeofday(&tv,0);
  
  to64(&salt[0], random(), 3);
  to64(&salt[3], tv.tv_usec, 3);
  salt[5] = '\0';

  return crypt (passwd, salt);
}

/* VTY enable password set. */
DEFUN (config_enable_password, enable_password_cmd,
       "enable password (8|) WORD",
       "Modify enable password parameters\n"
       "Assign the privileged level password\n"
       "Specifies a HIDDEN password will follow\n"
       "The HIDDEN 'enable' password string\n")
{
	/* Argument check. */
	if (argc == 0)
	{
		vty_out (vty, "Please specify password.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* Crypt type is specified. */
	if (argc == 2)
	{
		if (*argv[0] == '8')
		{
			if (host.enable)
				XFREE (0, host.enable);
			host.enable = NULL;

			if (host.enable_encrypt)
				XFREE (0, host.enable_encrypt);
			host.enable_encrypt = XSTRDUP (0, argv[1]);

			config_del_line_byleft(config_top, "enable password");
			config_add_line(config_top, "enable password 8 %s", host.enable_encrypt);
			return CMD_SUCCESS;
		}
		else
		{
			vty_out (vty, "Unknown encryption type.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	if (!isalnum ((int) *argv[0]))
	{
		vty_out (vty, "Please specify string starting with alphanumeric%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (host.enable)
		XFREE (0, host.enable);
	host.enable = NULL;

	if (host.enable_encrypt)
		XFREE (0, host.enable_encrypt);
	host.enable_encrypt = NULL;

	/* Plain password input. */
	config_del_line_byleft(config_top, "enable password");
	if (host.encrypt)
	{
		host.enable_encrypt = XSTRDUP (0, zencrypt (argv[0]));
		config_add_line(config_top, "enable password 8 %s", host.enable_encrypt);
	}
	else 
	{
		host.enable = XSTRDUP (0, argv[0]);
		config_add_line(config_top, "enable password %s", host.enable);
	}
	return CMD_SUCCESS;
}

ALIAS (config_enable_password,
       enable_password_text_cmd,
       "enable password LINE",
       "Modify enable password parameters\n"
       "Assign the privileged level password\n"
       "The UNENCRYPTED (cleartext) 'enable' password\n");

/* VTY enable password delete. */
DEFUN (no_config_enable_password, no_enable_password_cmd,
       "no enable password",
       NO_STR
       "Modify enable password parameters\n"
       "Assign the privileged level password\n")
{
	config_del_line_byleft(config_top, "enable password");

	if (host.enable)
		XFREE (0, host.enable);
	host.enable = NULL;

	if (host.enable_encrypt)
		XFREE (0, host.enable_encrypt);
	host.enable_encrypt = NULL;
  	
  return CMD_SUCCESS;
}

DEFUN (config_password, password_cmd,
       "password (8|) WORD",
       "Modify password parameters\n"
       "Specifies a HIDDEN password will follow\n"
       "The HIDDEN password string\n")
{
	/* Argument check. */
	if (argc == 0)
	{
		vty_out (vty, "Please specify password.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* Crypt type is specified. */
	if (argc == 2)
	{
		if (*argv[0] == '8')
		{
			if(vty->type != VTY_FILE)
			{
				vty_out(vty, "encrypt password is not supported!\n");
				return CMD_ERR_NOTHING_TODO;
			}
			if(host.password)
				XFREE (0, host.password);
			host.password = NULL;

			if (host.password_encrypt)
				XFREE (0, host.password_encrypt);
			host.password_encrypt = XSTRDUP (0, argv[1]);

			config_del_line_byleft(config_top, "password ");
			config_add_line(config_top, "password 8 %s", host.password_encrypt);
			return CMD_SUCCESS;
		}
		else
		{
			vty_out (vty, "Unknown encryption type.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	if (!isalnum ((int) *argv[0]))
	{
		vty_out (vty, "Please specify string starting with alphanumeric%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (host.password)
		XFREE (0, host.password);
	host.password = NULL;

	if (host.password_encrypt)
		XFREE (0, host.password_encrypt);
	host.password_encrypt = XSTRDUP (0, argv[1]);

	/* Plain password input. */
	config_del_line_byleft(config_top, "password ");
	if (host.encrypt)
	{
		host.password_encrypt = XSTRDUP (0, zencrypt (argv[0]));
		config_add_line(config_top, "password 8 %s", host.password_encrypt);
	}
	else 
	{
		host.password = XSTRDUP (0, argv[0]);
		config_add_line(config_top, "password %s", host.password);
	}
	if(vty->type == VTY_SHELL)
	{
		char mycmd[1024];
		// change the system password
		sprintf(mycmd, "echo admin:%s|chpasswd > /dev/null 2>&1 &", argv[0]);
		system(mycmd);
	}
	return CMD_SUCCESS;
}

ALIAS (config_password,
       password_text_cmd,
       "password LINE",
       "Modify password parameters\n"
       "The UNENCRYPTED (cleartext) password\n");

DEFUN (service_password_encrypt,
       service_password_encrypt_cmd,
       "service password-encryption",
       "Set up miscellaneous service\n"
       "Enable encrypted passwords\n")
{
	if (host.encrypt)
		return CMD_SUCCESS;

	host.encrypt = 1;

	if (host.enable)
	{
		if (host.enable_encrypt)
			XFREE (0, host.enable_encrypt);
		host.enable_encrypt = XSTRDUP (0, zencrypt (host.enable));
		config_del_line_byleft(config_top, "enable password");
		config_add_line(config_top, "enable password 8 %s", host.enable_encrypt);
	}
	if(host.password)
	{
		if (host.password_encrypt)
			XFREE (0, host.password_encrypt);
		host.password_encrypt = XSTRDUP (0, zencrypt (host.password));
		config_del_line_byleft(config_top, "password");
		config_add_line(config_top, "password 8 %s", host.password_encrypt);
	}
	config_add_line(config_top, "service password-encryption");

	return CMD_SUCCESS;
}

DEFUN (no_service_password_encrypt,
       no_service_password_encrypt_cmd,
       "no service password-encryption",
       NO_STR
       "Disable password-encryption service\n"
       "Disable encrypted passwords\n")
{
	if (! host.encrypt)
		return CMD_SUCCESS;

	host.encrypt = 0;
	if (host.enable)
	{
		if (host.enable_encrypt)
			XFREE (0, host.enable_encrypt);
		host.enable_encrypt = NULL;
		config_del_line_byleft(config_top, "enable password");
		config_add_line(config_top, "enable password %s", host.enable);
	}
	if(host.password)
	{
		if (host.password_encrypt)
			XFREE (0, host.password_encrypt);
		host.password_encrypt = NULL;
		config_del_line_byleft(config_top, "password ");
		config_add_line(config_top, "password %s", host.password);
	}
	config_del_line(config_top, "service password-encryption");	
	return CMD_SUCCESS;
}

/* Hostname configuration */
DEFUN (config_boot_image, 
       config_boot_image_cmd,
       "boot WORD",
       "Use the new image to boot device\n"
       "The new image name\n")
{
	char *myargv[10];
	char filename[1024];

	sprintf(filename, "%s/%s", CONFIG_DIR, argv[0]);
	if(access(filename, R_OK) != 0)
	{
		vty_out(vty, "the image file '%s' dose not exits\n", argv[0]);
		return CMD_ERR_NOTHING_TODO;
	}
	myargv[0] = filename;
	return cmd_execute_system_command("switchasm", 1, myargv);
}

int cmd_common_init()
{
  /* Each node's basic commands. */
  cmd_install_element (VIEW_NODE, &show_version_cmd);
  cmd_install_element (VIEW_NODE, &config_exit_cmd);
  cmd_install_element (VIEW_NODE, &config_enable_cmd);

  cmd_install_element (ENABLE_NODE, &show_version_cmd);
  cmd_install_element (ENABLE_NODE, &config_exit_cmd);
  cmd_install_element (ENABLE_NODE, &config_list_cmd);
  cmd_install_element (ENABLE_NODE, &config_write_terminal_cmd);
  cmd_install_element (ENABLE_NODE, &config_write_file_cmd);
  cmd_install_element (ENABLE_NODE, &config_write_memory_cmd);
  cmd_install_element (ENABLE_NODE, &config_write_cmd);
  cmd_install_element (ENABLE_NODE, &show_running_config_cmd);
  cmd_install_element (ENABLE_NODE, &config_disable_cmd);
  cmd_install_element (ENABLE_NODE, &config_terminal_cmd);
  cmd_install_element (ENABLE_NODE, &show_startup_config_cmd);
  cmd_install_element (ENABLE_NODE, &config_boot_image_cmd);

  cmd_install_element (CONFIG_NODE, &show_version_cmd);
  cmd_install_element (CONFIG_NODE, &config_exit_cmd);
  cmd_install_element (CONFIG_NODE, &config_end_cmd);
  cmd_install_element (CONFIG_NODE, &config_list_cmd);
  cmd_install_element (CONFIG_NODE, &config_write_terminal_cmd);
  cmd_install_element (CONFIG_NODE, &config_write_file_cmd);
  cmd_install_element (CONFIG_NODE, &config_write_memory_cmd);
  cmd_install_element (CONFIG_NODE, &config_write_cmd);
  cmd_install_element (CONFIG_NODE, &show_running_config_cmd);
  cmd_install_element (CONFIG_NODE, &show_startup_config_cmd);
  cmd_install_element (CONFIG_NODE, &hostname_cmd);
  cmd_install_element (CONFIG_NODE, &no_hostname_cmd);
//  cmd_install_element (CONFIG_NODE, &password_cmd);
//  cmd_install_element (CONFIG_NODE, &password_text_cmd);
  cmd_install_element (CONFIG_NODE, &enable_password_cmd);
  cmd_install_element (CONFIG_NODE, &enable_password_text_cmd);
  cmd_install_element (CONFIG_NODE, &no_enable_password_cmd);
  cmd_install_element (CONFIG_NODE, &service_password_encrypt_cmd);
  cmd_install_element (CONFIG_NODE, &no_service_password_encrypt_cmd);

  
  return 0;
}
