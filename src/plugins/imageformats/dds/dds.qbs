import qbs.base 1.0

DynamicLibrary {
    name : "qdds"
    destinationDirectory: "imageformats"

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "gui"] }

    files : [
        "dds.h",
        "dds.cpp",
        "ddsheader.h",
        "ddsheader.cpp",
        "dxt.h",
        "dxt.cpp"
    ]

    Group {
        name: "dds.rc"
        condition: qbs.targetOS.contains("windows")
        files: "dds.rc"
    }
}
