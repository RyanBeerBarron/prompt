/*
*		TODO: Need to have multiple patterns
*		Like a simple pattern with foreground and background colors
*		More emojis/nerd font icons ( success / error / signals )
*		A pattern with text (user in dir at branch.. blablabla.. xxxth command)
*		24-bit colors aka true colors
*		Show diff stat of git repo if within one else show for dotfiles repo
*		To do with git-cli? or libgit?
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define RGB(r,g,b)          "\x01\x1b[38;2;" #r ";" #g ";" #b "m\x02"
#define COLOR_8(c)          "\x01\x1b[3" #c "m\x02"
#define BRIGHT_COLOR_8(c)   "\x01\x1b[9" #c "m\x02"
#define COLOR_256(c)        "\x01\x1b[38;5;" #c "m\x02"

#define BOLD_MODE "\x01\x1b[1m\x02"
#define GIT_ICON_COLOR			RGB(241, 80, 47)
#define UBUNTU_ICON_COLOR		RGB(233, 84, 32)
#define PREPOSITION_COLOR       COLOR_256(240)

typedef struct colorscheme Colorscheme;
struct colorscheme {
    char *text_color;	char *username_color;	char *hostname_color;
    char *cwd_color;	char *time_color;	char *hist_count_color;
    char *git_color;	char *git_icon_color;	char *ubuntu_icon_color;
};
enum Colorschemes {
    KANAGAWA,
};
const Colorscheme colors[] = {
	[KANAGAWA] = {	.text_color = RGB(220, 213, 172),	.username_color	= RGB(220, 165, 97),	.hostname_color		= RGB(210,	 126, 153),
			.cwd_color	= RGB(135, 169, 135),	.time_color		= RGB(149, 127, 184),	.hist_count_color	= RGB(89,	 123, 117),
			.git_color	= RGB(255, 160,	102),
	},
};
const Colorscheme *colorscheme = &colors[KANAGAWA];

const char *usage_string = "\
usage: prints bash prompt with given information in order according to baked in pattern/colorscheme\n\
$ prompt [-h] [option [arg]]\n\
options:\n\
  -c <return code> print emoji for common exit code OR signal name if it corresponds to a signal\n\
  -d <distro> print icon of linux distro\n\
  -g <git_branch>\n\
  -H <hostname> \n\
  -n <bash history count>\n\
  -t <last command time>\n\
  -u <username>\n\
  -w <current working directory>\n\
";

void usage(int exit_code)
{
    printf("%s", usage_string);
    exit(exit_code);
}

void print_return_code(char* str)
{
	unsigned char rc = atoi(str);
    char *signame;
    int sig;
    // return code corresponds to a signals
    if(rc > 128) {
        sig = rc - 128;
        switch(sig) {
        case SIGINT:
            signame = "⚠️";
            break;
        case SIGPIPE:
            signame = "";
            break;
        case SIGTSTP:
            signame = "";
            break;
        default:
            signame = strsignal(sig);
            break;
        }

        printf(" %s", signame);
        return;
    }
    switch(rc) {
    case 0:
        //colorscheme = SUCCESS;
        str = "✅";
        break;
    case 1:
        //colorscheme = ERROR;
        str = "❌";
        break;
    case 126:
        //colorscheme = CANT_EXECUTE;
        str = "🚫";
        break;
    case 127:
        //colorscheme = COMMAND_NOT_FOUND;
        str = "❓";
        break;
    default:
        //colorscheme = DEFAULT_ERROR;
        break;
    }
    printf(" %s", str);
    return;
}

void print_hist_count(char *hist_count)
{
    printf("%s %s#", colorscheme->hist_count_color, hist_count);
}

void print_cwd(char *cwd)
{
    char *icon;
    switch(cwd[0]) {
    case '~':
        cwd = "home";
        icon = "";
        break;
    case '/':
        cwd = "root";
    default:
        icon = "";
        break;
    }
    printf("%s in %s%s %s",
           PREPOSITION_COLOR,
           colorscheme->cwd_color, icon, cwd);
}

void print_user(char *user)
{
    printf("%s%s", colorscheme->username_color, user);
}

void print_hostname(char *hostname)
{
    printf("%s on %s%s",
           PREPOSITION_COLOR,
           colorscheme->hostname_color, hostname);
}

void print_distro(char *distro)
{
    if(strstr(distro, "Ubuntu"))
        distro = UBUNTU_ICON_COLOR"";
    printf("%s ", distro);
}

void print_git_branch(char *git_branch)
{
    printf(" %sat %s%s%s%s",
           PREPOSITION_COLOR,
		   GIT_ICON_COLOR, "󰊢",
		   colorscheme->git_color, git_branch);
}

void print_git_stat(char *git_stat)
{
    char *endptr;
    int buf[3];
    for (int i=0; i<3; i++) {
        buf[i] = (int) strtol(git_stat, &endptr, 10);
        git_stat = endptr;
    }
    if(buf[0] == 0)
        return;
    printf(" %s~%d %s+%d %s-%d",
           COLOR_8(3), buf[0],
           COLOR_8(2), buf[1],
           COLOR_8(1), buf[2]);
}

void print_time_taken(char *time_taken)
{
    printf("%s took %s", colorscheme->time_color, time_taken);
}

int main(int argc, char **argv)
{
    // Could add a termcode to reset cursor to leftmost columns
    // In case a previous command got interrupted
    printf("      %s%s┏━ ", BOLD_MODE, colorscheme->text_color);
    for(int i=1; i<argc; i++) {
        char *arg = argv[i];
        if (arg[0] != '-') {
            fprintf(stderr, "Incorrect argument, did not start with dash: %s\n", arg);
            usage(1);
        }
        if (arg[1] == 'h')
            usage(0);
        if (++i >= argc) {
            fprintf(stderr, "Missing argument for option %s\n", arg);
            usage(1);
        }
        char option = arg[1];
        switch(option) {
        case 'c':
            print_return_code(argv[i]);
            break;
        case 'd':
            print_distro(argv[i]);
            break;
        case 'H':
            print_hostname(argv[i]);
            break;
        case 'g':
            if(argv[i][0]) {
                print_git_branch(argv[i]);
            }
            break;
        case 'n':
            print_hist_count(argv[i]);
            break;
        case 's':
            print_git_stat(argv[i]);
            break;
        case 't':
            print_time_taken(argv[i]);
            break;
        case 'u':
            print_user(argv[i]);
            break;
        case 'w':
            print_cwd(argv[i]);
            break;
        case '-':
            // print the rest on the right side
            break;
        default:
            fprintf(stderr, "Unrecognized option flag: %s\n", arg);
            usage(1);
        }
    }
    printf("\n%s┗━ $ ", colorscheme->text_color);
}
