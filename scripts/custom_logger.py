import logging
import sys

RESET = "\033[0m"
COLORS = {
    "TIME": "\033[32m",   # Green
    "DEBUG": "\033[36m",  # Cyan
    "INFO": "\033[97m",   # White
    "WARNING": "\033[33m",# Yellow
    "ERROR": "\033[31m",  # Red
    "CRITICAL": "\033[41m\033[97m", # Red background, white text
    "FILENAME": "\033[34m",  # Blue
    "MESSAGE": "\033[97m"    # White
}

class ColoredFormatter(logging.Formatter):
    def format(self, record):
        asctime = f"{COLORS['TIME']}{self.formatTime(record, self.datefmt)}{RESET}"
        levelname_plain = f"{record.levelname:<8}"
        level_color = COLORS.get(record.levelname.strip(), "")
        level_colored = f"\033[1m{level_color}{levelname_plain}{RESET}"
        module_colored = f"{COLORS['FILENAME']}{record.module}{RESET}"
        func_colored = f"\033[35m{record.funcName}{RESET}"
        lineno_colored = f"\033[36m{record.lineno}{RESET}"
        msg_colored = f"\033[1m{COLORS['MESSAGE']}{record.getMessage()}{RESET}"
        return f"{asctime} | {level_colored} | {module_colored}:{func_colored}:{lineno_colored} - {msg_colored}"


def get_logger(name=__name__, level=logging.INFO):
    logger = logging.getLogger(name)
    logger.setLevel(level)
    handler = logging.StreamHandler(sys.stdout)
    formatter = ColoredFormatter()
    handler.setFormatter(formatter)
    logger.handlers = [handler]
    return logger
