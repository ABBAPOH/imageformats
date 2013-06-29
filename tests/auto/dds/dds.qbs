import qbs.base 1.0

Product {
    type: "application"
    name: "tst_dds"
    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "gui", "test"] }
    files: [
        "tst_dds.cpp",
        "tst_dds.qrc"
    ]
}
