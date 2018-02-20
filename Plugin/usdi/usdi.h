#pragma once

#ifdef _WIN32
    #ifndef usdiStaticLink
        #ifdef usdiImpl
            #define usdiAPI __declspec(dllexport)
        #else
            #define usdiAPI __declspec(dllimport)
        #endif
    #else
        #define usdiAPI
    #endif
    #define usdiSTDCall __stdcall
#else
    #define usdiAPI
    #define usdiSTDCall
#endif

#include <cstdint>

namespace usdi {

using byte = unsigned char;
using uint = unsigned int;
#ifndef usdiImpl
    // force make compatible
    class Context {};
    class Attribute {};
    class Schema {};
    class Xform : public Schema  {};
    class Camera : public Xform {};
    class Mesh : public Xform {};
    class Points : public Xform {};

#ifndef usdiOverrideFloat4
    struct float2
    {
        float x, y;
        static constexpr float2 zero() { return{ 0,0 }; }
    };
    struct float3
    {
        float x, y, z;
        static constexpr float3 zero() { return{ 0,0,0 }; }
        static constexpr float3 one() { return{ 1,1,1 }; }
    };
    struct float4
    {
        float x, y, z, w;
    };
    struct quatf
    {
        float x, y, z, w;
        static constexpr quatf identity() { return{ 0,0,0,1 }; }
    };
    struct float3x3
    {
        float3 v[3];
        static constexpr float3x3 identity()
        {
            return{ {
                { 1,0,0 },
                { 0,1,0 },
                { 0,0,1 },
            } };
        }
    };
    struct float4x4
    {
        float4 v[4];
        static constexpr float4x4 identity()
        {
            return{ {
                { 1,0,0,0 },
                { 0,1,0,0 },
                { 0,0,1,0 },
                { 0,0,0,1 },
            } };
        }
    };
#endif
#endif
    struct AABB
    {
        float3 center, extents;
    };


enum class InterpolationType
{
    None,
    Linear,
};

enum class NormalCalculationType
{
    Never,
    WhenMissing,
    Always,
};
enum class TangentCalculationType
{
    Never,
    Always,
};

enum class AttributeType
{
    Unknown,
    Bool, Byte, Int, UInt, Int64, UInt64,
    Half, Half2, Half3, Half4, QuatH,
    Float, Float2, Float3, Float4, QuatF,
    Double, Double2, Double3, Double4, QuatD,
    Float2x2, Float3x3, Float4x4,
    Double2x2, Double3x3, Double4x4,
    String, Token, Asset,

    UnknownArray = 0x100,
    BoolArray, ByteArray, IntArray, UIntArray, Int64Array, UInt64Array,
    HalfArray, Half2Array, Half3Array, Half4Array, QuatHArray,
    FloatArray, Float2Array, Float3Array, Float4Array, QuatFArray,
    DoubleArray, Double2Array, Double3Array, Double4Array, QuatDArray,
    Float2x2Array, Float3x3Array, Float4x4Array,
    Double2x2Array, Double3x3Array, Double4x4Array,
    StringArray, TokenArray, AssetArray,
};

enum class TopologyVariance
{
    Constant, // both vertices and topologies are constant
    Homogenous, // vertices are not constant (= animated). topologies are constant.
    Heterogenous, // both vertices and topologies are not constant
};

union UpdateFlags {
    struct {
        uint sample_updated : 1;
        uint import_settings_updated : 1;
        uint variant_set_changed : 1;
        uint payload_loaded : 1;
        uint payload_unloaded : 1;
    };
    uint bits;
};

typedef double Time;

struct ImportSettings
{
    InterpolationType interpolation = InterpolationType::Linear;
    NormalCalculationType normal_calculation = NormalCalculationType::WhenMissing;
    TangentCalculationType tangent_calculation = TangentCalculationType::Always;
    int split_unit = 0x7fffffff;
    int max_bone_weights = 0; // must be 0 or 4 or 8.
    float scale_factor = 1.0f;
    bool load_all_payloads = true;
    bool triangulate = true;
    bool swap_handedness = false;
    bool swap_faces = false;
};

struct ExportSettings
{
    float scale_factor = 1.0f;
    bool swap_handedness = false;
    bool swap_faces = false;
    bool instanceable_by_default = false;
};


struct XformSummary
{
    enum class Type {
        Unknown,
        TRS,
        Matrix,
    };

    Time start = 0.0, end = 0.0;
    Type type = Type::Unknown;
};

struct XformData
{
    enum class Flags {
        UpdatedMask     = 0xf,
        UpdatedPosition = 0x1,
        UpdatedRotation = 0x2,
        UpdatedScale    = 0x4,
    };

    int flags = 0;
    float3 position = float3::zero();
    quatf rotation = quatf::identity();
    float3 scale = float3::one();
    float4x4 transform = float4x4::identity();
};


struct CameraSummary
{
    Time start = 0.0, end = 0.0;
};

struct CameraData
{
    float near_clipping_plane = 0.3f;
    float far_clipping_plane = 1000.0f;
    float field_of_view = 60.0f;        // in degree. vertical one
    float aspect_ratio = 16.0f / 9.0f;

    float focus_distance = 5.0f;        // in cm
    float focal_length = 0.0f;          // in mm
    float aperture = 35.0f;             // in mm. vertical one
};


struct MeshSummary
{
    Time start = 0.0, end = 0.0;
    TopologyVariance topology_variance = TopologyVariance::Constant;
    uint bone_count = 0;
    uint max_bone_weights = 0; // should be 0 or 4 or 8
    bool has_velocities = false;
    bool has_normals = false;
    bool has_tangents = false;
    bool has_uv0 = false;
    bool has_uv1 = false;
    bool has_colors = false;
    bool constant_points = false;
    bool constant_velocities = false;
    bool constant_normals = false;
    bool constant_tangents = false;
    bool constant_uv0 = false;
    bool constant_uv1 = false;
    bool constant_colors = false;
};

struct MeshSampleSummary
{
    int split_count = 0;
    int submesh_count = 0;
    int vertex_count = 0;
    int index_count = 0;
    bool topology_changed = false;
};

struct MeshSplitSummary
{
    int submesh_count = 0;
    int submesh_offset = 0;
    int vertex_count = 0;
    int vertex_offset = 0;
    int index_count = 0;
    int index_offset = 0;
};


template<int N>
struct Weights
{
    float weight[N] = {};
    int   indices[N] = {};
};
using Weights4 = Weights<4>;
using Weights8 = Weights<8>;

struct MeshData
{
    float3 *points = nullptr;
    float3 *velocities = nullptr;
    float3 *normals = nullptr;
    float4 *tangents = nullptr;
    float2 *uv0 = nullptr;
    float2 *uv1 = nullptr;
    float4 *colors = nullptr;
    int    *indices = nullptr;
    int    *faces = nullptr;
    uint   vertex_count = 0;
    uint   index_count = 0;
    uint   face_count = 0;

    // bounds
    float3 center = float3::zero();
    float3 extents = float3::zero();

    // bone & weights
    union {
        Weights4 *weights4 = nullptr;
        Weights8 *weights8;
    };
    float4x4 *bindposes = nullptr;
    char     **bones = nullptr;
    char     *root_bone = nullptr;
    int      bone_count = 0;
    int      max_bone_weights = 0;
};

struct SubmeshData
{
    int *indices = nullptr;
};


struct PointsSummary
{
    Time    start = 0.0, end = 0.0;
    bool    has_velocities = false;
};

struct PointsData
{
    // these pointers can be null (in this case, just be ignored).
    // otherwise, if you pass to usdiMeshSampleReadData(), pointers must point valid memory block to store data.
    float3  *points = nullptr;
    float3  *velocities = nullptr;
    float   *widths = nullptr;
    int64_t *ids64 = nullptr;
    int32_t *ids32 = nullptr;

    uint    num_points = 0;
};


struct AttributeSummary
{
    Time            start = 0.0, end = 0.0;
    AttributeType   type = usdi::AttributeType::Unknown;
    int             num_samples = 0;
};

struct AttributeData
{
    void    *data = nullptr;
    int     num_elements = 0;
};

} // namespace usdi

extern "C" {

usdiAPI void             usdiSetDebugLevel(int l);
usdiAPI usdi::Time       usdiDefaultTime();

usdiAPI void             usdiInitialize();
usdiAPI void             usdiFinalize();

usdiAPI void*            usdiAlignedMalloc(size_t size, size_t alignment);
usdiAPI void             usdiAlignedFree(void *addr);

usdiAPI void             usdiAddAssetSearchPath(const char *path);
usdiAPI void             usdiClearAssetSearchPath();

// Context interface
usdiAPI void             usdiClearContextsWithPath(const char *path);
usdiAPI usdi::Context*   usdiCreateContext(int uid = -1);
usdiAPI void             usdiDestroyContext(usdi::Context *ctx);
usdiAPI bool             usdiOpen(usdi::Context *ctx, const char *path);
usdiAPI bool             usdiCreateStage(usdi::Context *ctx, const char *path);
usdiAPI void             usdiFlatten(usdi::Context *ctx);
usdiAPI bool             usdiSave(usdi::Context *ctx);
// path must *not* be same as identifier (parameter of usdiOpen() or usdiCreateStage())
usdiAPI bool             usdiSaveAs(usdi::Context *ctx, const char *path);
usdiAPI void             usdiSetImportSettings(usdi::Context *ctx, const usdi::ImportSettings *v);
usdiAPI void             usdiGetImportSettings(usdi::Context *ctx, usdi::ImportSettings *v);
usdiAPI void             usdiSetExportSettings(usdi::Context *ctx, const usdi::ExportSettings *v);
usdiAPI void             usdiGetExportSettings(usdi::Context *ctx, usdi::ExportSettings *v);

usdiAPI usdi::Schema*    usdiGetRoot(usdi::Context *ctx);
usdiAPI int              usdiGetNumSchemas(usdi::Context *ctx);
usdiAPI usdi::Schema*    usdiGetSchema(usdi::Context *ctx, int i);
usdiAPI int              usdiGetNumMasters(usdi::Context *ctx);
usdiAPI usdi::Schema*    usdiGetMaster(usdi::Context *ctx, int i);
usdiAPI usdi::Schema*    usdiFindSchema(usdi::Context *ctx, const char *path_or_name);

usdiAPI usdi::Schema*    usdiCreateOverride(usdi::Context *ctx, const char *prim_path);
usdiAPI usdi::Xform*     usdiCreateXform(usdi::Context *ctx, usdi::Schema *parent, const char *name);
usdiAPI usdi::Camera*    usdiCreateCamera(usdi::Context *ctx, usdi::Schema *parent, const char *name);
usdiAPI usdi::Mesh*      usdiCreateMesh(usdi::Context *ctx, usdi::Schema *parent, const char *name);
usdiAPI usdi::Points*    usdiCreatePoints(usdi::Context *ctx, usdi::Schema *parent, const char *name);

usdiAPI void             usdiNotifyForceUpdate(usdi::Context *ctx);
usdiAPI void             usdiUpdateAllSamples(usdi::Context *ctx, usdi::Time t);
usdiAPI void             usdiRebuildSchemaTree(usdi::Context *ctx);

// Prim interface
usdiAPI int              usdiPrimGetID(usdi::Schema *schema);
usdiAPI const char*      usdiPrimGetPath(usdi::Schema *schema);
usdiAPI const char*      usdiPrimGetName(usdi::Schema *schema);
usdiAPI const char*      usdiPrimGetUsdTypeName(usdi::Schema *schema);

usdiAPI usdi::Schema*    usdiPrimGetMaster(usdi::Schema *schema);
usdiAPI int              usdiPrimGetNumInstances(usdi::Schema *schema);
usdiAPI usdi::Schema*    usdiPrimGetInstance(usdi::Schema *schema, int i);
usdiAPI bool             usdiPrimIsEditable(usdi::Schema *schema);
usdiAPI bool             usdiPrimIsInstance(usdi::Schema *schema);
usdiAPI bool             usdiPrimIsInstanceable(usdi::Schema *schema);
usdiAPI bool             usdiPrimIsMaster(usdi::Schema *schema);
usdiAPI bool             usdiPrimIsInMaster(usdi::Schema *schema);
usdiAPI void             usdiPrimSetInstanceable(usdi::Schema *schema, bool v);
// create external reference if asset_path is valid, otherwise create internal reference
usdiAPI bool             usdiPrimAddReference(usdi::Schema *schema, const char *asset_path, const char *prim_path);

usdiAPI bool             usdiPrimHasPayload(usdi::Schema *schema);
usdiAPI void             usdiPrimLoadPayload(usdi::Schema *schema);
usdiAPI void             usdiPrimUnloadPayload(usdi::Schema *schema);
usdiAPI bool             usdiPrimSetPayload(usdi::Schema *schema, const char *asset_path, const char *prim_path);

usdiAPI usdi::Schema*    usdiPrimGetParent(usdi::Schema *schema);
usdiAPI int              usdiPrimGetNumChildren(usdi::Schema *schema);
usdiAPI usdi::Schema*    usdiPrimGetChild(usdi::Schema *schema, int i);
usdiAPI usdi::Schema*    usdiPrimFindChild(usdi::Schema *schema, const char *path_or_name, bool recursive = true);

usdiAPI int              usdiPrimGetNumAttributes(usdi::Schema *schema);
usdiAPI usdi::Attribute* usdiPrimGetAttribute(usdi::Schema *schema, int i);
usdiAPI usdi::Attribute* usdiPrimFindAttribute(usdi::Schema *schema, const char *name, usdi::AttributeType type = usdi::AttributeType::Unknown);
usdiAPI usdi::Attribute* usdiPrimCreateAttribute(usdi::Schema *schema, const char *name, usdi::AttributeType type, usdi::AttributeType internal_type = usdi::AttributeType::Unknown);

usdiAPI int              usdiPrimGetNumVariantSets(usdi::Schema *schema);
usdiAPI int              usdiPrimGetNumVariants(usdi::Schema *schema, int iset);
usdiAPI const char*      usdiPrimGetVariantName(usdi::Schema *schema, int iset, int ival);
usdiAPI int              usdiPrimGetVariantSelection(usdi::Schema *schema, int iset);
// clear selection if ival is invalid value
usdiAPI bool             usdiPrimSetVariantSelection(usdi::Schema *schema, int iset, int ival);
// return -1 if not found
usdiAPI int              usdiPrimFindVariantSet(usdi::Schema *schema, const char *name);
// return -1 if not found
usdiAPI int              usdiPrimFindVariant(usdi::Schema *schema, int iset, const char *name);
usdiAPI bool             usdiPrimBeginEditVariant(usdi::Schema *schema, const char *set, const char *variant);
usdiAPI void             usdiPrimEndEditVariant(usdi::Schema *schema);

usdiAPI usdi::UpdateFlags usdiPrimGetUpdateFlags(usdi::Schema *schema);
usdiAPI usdi::UpdateFlags usdiPrimGetUpdateFlagsPrev(usdi::Schema *schema);
usdiAPI void             usdiPrimUpdateSample(usdi::Schema *schema, usdi::Time t);
usdiAPI void*            usdiPrimGetUserData(usdi::Schema *schema);
usdiAPI void             usdiPrimSetUserData(usdi::Schema *schema, void *data);

// Xform interface
usdiAPI usdi::Xform*     usdiAsXform(usdi::Schema *schema); // dynamic cast to Xform
usdiAPI void             usdiXformGetSummary(usdi::Xform *xf, usdi::XformSummary *dst);
usdiAPI bool             usdiXformReadSample(usdi::Xform *xf, usdi::XformData *dst);
usdiAPI bool             usdiXformWriteSample(usdi::Xform *xf, const usdi::XformData *src, usdi::Time t = usdiDefaultTime());
using usdiXformSampleCallback = void (usdiSTDCall*)(const usdi::XformData *data, usdi::Time t);
usdiAPI int              usdiXformEachSample(usdi::Xform *xf, usdiXformSampleCallback cb);

// Camera interface
usdiAPI usdi::Camera*    usdiAsCamera(usdi::Schema *schema); // dynamic cast to Camera
usdiAPI void             usdiCameraGetSummary(usdi::Camera *cam, usdi::CameraSummary *dst);
usdiAPI bool             usdiCameraReadSample(usdi::Camera *cam, usdi::CameraData *dst);
usdiAPI bool             usdiCameraWriteSample(usdi::Camera *cam, const usdi::CameraData *src, usdi::Time t = usdiDefaultTime());
using usdiCameraSampleCallback = void (usdiSTDCall*)(const usdi::CameraData *data, usdi::Time t);
usdiAPI int              usdiCameraEachSample(usdi::Camera *cam, usdiCameraSampleCallback cb);

// Mesh interface
usdiAPI usdi::Mesh*      usdiAsMesh(usdi::Schema *schema); // dynamic cast to Mesh
usdiAPI void             usdiMeshGetSummary(usdi::Mesh *mesh, usdi::MeshSummary *dst);
usdiAPI bool             usdiMeshReadSample(usdi::Mesh *mesh, usdi::MeshData *dst);
usdiAPI bool             usdiMeshWriteSample(usdi::Mesh *mesh, const usdi::MeshData *src, usdi::Time t = usdiDefaultTime());
using usdiMeshSampleCallback = void (usdiSTDCall*)(const usdi::MeshData *data, usdi::Time t);
usdiAPI int              usdiMeshEachSample(usdi::Mesh *mesh, usdiMeshSampleCallback cb);

// Points interface
usdiAPI usdi::Points*    usdiAsPoints(usdi::Schema *schema); // dynamic cast to Points
usdiAPI void             usdiPointsGetSummary(usdi::Points *points, usdi::PointsSummary *dst);
usdiAPI bool             usdiPointsReadSample(usdi::Points *points, usdi::PointsData *dst);
usdiAPI bool             usdiPointsWriteSample(usdi::Points *points, const usdi::PointsData *src, usdi::Time t = usdiDefaultTime());
using usdiPointsSampleCallback = void (usdiSTDCall*)(const usdi::PointsData *data, usdi::Time t);
usdiAPI int              usdiPointsEachSample(usdi::Points *points, usdiPointsSampleCallback cb);

// Attribute interface
usdiAPI usdi::Schema*    usdiAttrGetParent(usdi::Attribute *attr);
usdiAPI const char*      usdiAttrGetName(usdi::Attribute *attr);
usdiAPI const char*      usdiAttrGetTypeName(usdi::Attribute *attr);
usdiAPI void             usdiAttrGetSummary(usdi::Attribute *attr, usdi::AttributeSummary *dst);
usdiAPI bool             usdiAttrReadSample(usdi::Attribute *attr, usdi::AttributeData *dst, usdi::Time t);
usdiAPI bool             usdiAttrWriteSample(usdi::Attribute *attr, const usdi::AttributeData *src, usdi::Time t = usdiDefaultTime());

// Alembic related
usdiAPI bool             usdiConvertUSDToAlembic(const char *src_usd, const char *dst_abc);


} // extern "C"
