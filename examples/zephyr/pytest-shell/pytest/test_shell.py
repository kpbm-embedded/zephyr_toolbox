# Twister pytest harness: twister builds the app, boots it (qemu here, a board with --device-testing)
# and hands these tests a `shell` fixture connected to its console (pytest-twister-harness).
import re

from twister_harness import Shell


def test_demo_hello(shell: Shell):
    lines = shell.exec_command('demo hello robot')
    assert 'hello robot' in lines


def test_demo_uptime_grows(shell: Shell):
    first = int(re.search(r'uptime (\d+) ms', '\n'.join(shell.exec_command('demo uptime'))).group(1))
    second = int(re.search(r'uptime (\d+) ms', '\n'.join(shell.exec_command('demo uptime'))).group(1))
    assert second >= first


def test_kernel_version(shell: Shell):
    assert any('Zephyr version' in line for line in shell.exec_command('kernel version'))
