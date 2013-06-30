import qbs.base 1.0

DynamicLibrary {
    name : "qicns"

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "gui"] }

    files : [
        "main.h",
        "main.cpp",
        "icnsheader.h",
        "qicnshandler.h",
        "qicnshandler.cpp"
    ]

    Group {
        name: "icns.rc"
        condition: qbs.targetOS.contains("windows")
        files: "icns.rc"
    }
}
