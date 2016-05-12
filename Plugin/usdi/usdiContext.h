#pragma once

namespace usdi {

class Context
{
public:
    Context();
    virtual ~Context();

    bool                valid() const;
    void                unload();
    void                create(const char *identifier);
    bool                open(const char *path);
    bool                write(const char *path);

    const ImportConfig& getImportConfig() const;
    void                setImportConfig(const ImportConfig& v);
    const ExportConfig& getExportConfig() const;
    void                setExportConfig(const ExportConfig& v);

    Schema*             getRootNode();

private:
    void    constructTreeRecursive(Schema *parent, UsdPrim prim);
    Schema* createNode(Schema *parent, UsdPrim prim);

private:
    typedef std::map<std::string, std::string> Variants;
    typedef std::unique_ptr<Schema> SchemaPtr;
    typedef std::vector<SchemaPtr> Schemas;

    UsdStageRefPtr  m_stage;
    Schemas         m_schemas;
    std::string     m_prim_root;
    Variants        m_variants;

    ImportConfig    m_import_config;
    ExportConfig    m_export_config;

    double          m_start_time = 0.0;
    double          m_end_time = 0.0;
};

} // namespace usdi