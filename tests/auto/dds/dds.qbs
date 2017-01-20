import qbs.base 1.0

Product {
    destinationDirectory: "."
    type: "application"
    name: "tst_dds"
    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "gui", "test"] }
    files: [
        "data/data.qrc",
        "tst_qdds.cpp"
    ]
}
