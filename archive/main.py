from pathlib import Path
import hashlib
import re
import zipfile


REQUIRED_ITEMS = [
    "test",
    "src",
    "include",
    "example",
    "build/linux/x86_64/release",
    "report/main.pdf",
    "xmake.lua",
]


def add_to_zip(base_dir: Path, relative_path: str, zf: zipfile.ZipFile) -> bool:
    """Add a required file or directory under base_dir into zf."""
    target = base_dir / relative_path
    if not target.exists():
        return False

    if target.is_file():
        arcname = target.relative_to(base_dir).as_posix()
        zf.write(target, arcname)
        return True

    added = False
    for child in target.rglob("*"):
        if child.is_file():
            arcname = child.relative_to(base_dir).as_posix()
            zf.write(child, arcname)
            added = True
    return added


def file_hash(path: Path, algorithm: str) -> str:
    h = hashlib.new(algorithm)
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def human_readable_size(size_bytes: int) -> str:
    units = ["B", "KB", "MB", "GB", "TB"]
    size = float(size_bytes)
    for unit in units:
        if size < 1024 or unit == units[-1]:
            return f"{size:.2f} {unit}" if unit != "B" else f"{int(size)} {unit}"
        size /= 1024
    return f"{int(size_bytes)} B"


def archive_numeric_experiments() -> None:
    script_dir = Path(__file__).resolve().parent
    project_root = script_dir.parent
    experiment_dir = project_root / "experiment"

    if not experiment_dir.is_dir():
        raise FileNotFoundError(f"Cannot find experiment directory: {experiment_dir}")

    numeric_dirs = sorted(
        p
        for p in experiment_dir.iterdir()
        if p.is_dir() and re.fullmatch(r"\d+", p.name)
    )

    if not numeric_dirs:
        print(f"No numeric subdirectories found in: {experiment_dir}")
        return

    created_zips: list[Path] = []

    for exp_dir in numeric_dirs:
        zip_path = script_dir / f"{exp_dir.name}.zip"
        included_any = False
        missing_items: list[str] = []

        with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as zf:
            for item in REQUIRED_ITEMS:
                if add_to_zip(exp_dir, item, zf):
                    included_any = True
                else:
                    missing_items.append(item)

        if included_any:
            print(f"Created: {zip_path}")
            created_zips.append(zip_path)
        else:
            zip_path.unlink(missing_ok=True)
            print(f"Skipped {exp_dir.name}: no required content found")

        if missing_items:
            print(f"  Missing in {exp_dir.name}: {', '.join(missing_items)}")

    if created_zips:
        print("\nSummary (size / md5 / sha256):")
        for zip_path in created_zips:
            size = human_readable_size(zip_path.stat().st_size)
            md5 = file_hash(zip_path, "md5")
            sha256 = file_hash(zip_path, "sha256")
            print(f"- {zip_path.name}")
            print(f"  size: {size}")
            print(f"  md5: {md5}")
            print(f"  sha256: {sha256}")


if __name__ == "__main__":
    archive_numeric_experiments()
