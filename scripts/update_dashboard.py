import subprocess
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
README = ROOT / "README.md"

MEMBERS = [
    ("Md Mostafizur Rahman", "Raspberry Pi 3 — Central Controller", "01_RaspberryPi_Central_Controller_MdMostafizurRahman/"),
    ("Turja Barua", "ESP32-1 — Environment Monitoring Node", "02_ESP32_Environment_Node_TurjaBarua/"),
    ("Moaj Chowdhury", "ESP32-2 — Fire and Gas Safety Node", "03_ESP32_FireGas_Safety_Node_MoajChowdhury/"),
    ("Deepak Kapil", "Arduino Uno WiFi Rev2 — Actuator / Relay Node", "04_Arduino_Relay_Node_DeepakKapil/"),
]


def run_git(args):
    try:
        result = subprocess.run(
            ["git"] + args,
            cwd=ROOT,
            text=True,
            capture_output=True,
            check=False,
        )
        return result.stdout.strip()
    except Exception:
        return ""


def commit_count(folder):
    output = run_git(["log", "--oneline", "--", folder])
    if not output:
        return 0
    return len(output.splitlines())


def last_activity(folder):
    output = run_git(["log", "-1", "--format=%cs by %an", "--", folder])
    return output if output else "Not updated yet"


def progress_from_commits(count):
    return min(count * 20, 100)


def build_dashboard():
    lines = []
    lines.append("| Member | Responsible Node | Folder | Commits | Last Activity | Progress |")
    lines.append("|---|---|---|---:|---|---|")
    for member, node, folder in MEMBERS:
        count = commit_count(folder)
        activity = last_activity(folder)
        progress = progress_from_commits(count)
        lines.append(f"| {member} | {node} | `{folder}` | {count} | {activity} | {progress}% |")
    return "\n".join(lines)


def main():
    text = README.read_text(encoding="utf-8")
    dashboard = build_dashboard()
    new_text = re.sub(
        r"<!-- AUTO_DASHBOARD_START -->.*?<!-- AUTO_DASHBOARD_END -->",
        f"<!-- AUTO_DASHBOARD_START -->\n{dashboard}\n<!-- AUTO_DASHBOARD_END -->",
        text,
        flags=re.DOTALL,
    )
    README.write_text(new_text, encoding="utf-8")


if __name__ == "__main__":
    main()
