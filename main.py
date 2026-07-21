import sys
import argparse
from pathlib import Path

from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine


def main():
    # Parse our own argument before Qt sees argv
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--import-path",
        default=None,
        help="Path to directory containing MeshPlugin folder"
    )
    args, qt_args = parser.parse_known_args()

    app = QGuiApplication([sys.argv[0]] + qt_args)
    engine = QQmlApplicationEngine()

    # Determine import path — CLI arg wins, then installed location, then build dir
    if args.import_path:
        import_path = Path(args.import_path)
    elif (Path(__file__).parent.parent / "lib" / "MeshPlugin").exists():
        import_path = Path(__file__).parent.parent / "lib"
    else:
        import_path = Path(__file__).parent.parent.parent / "build" / "imports"

    engine.addImportPath(str(import_path))

    qml_file = Path(__file__).parent / "qml" / "main.qml"
    engine.load(str(qml_file))

    if not engine.rootObjects():
        print("Failed to load QML")
        sys.exit(1)

    sys.exit(app.exec())


if __name__ == "__main__":
    main()