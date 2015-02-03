#include <HAPI/HAPI.h>
#include <HAPI_cpp.h>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

namespace hapi {

Engine* Engine::sInstance = nullptr;

std::string getString( int string_handle )
{
    if (string_handle == 0)
        return "";

    int buffer_length;
    HAPI_GetStringBufLength(string_handle, &buffer_length);

    char * buf = new char[ buffer_length ];

    HAPI_GetString(string_handle, buf, buffer_length);

    std::string result( buf );
    delete[] buf;
    return result;
}

static void throwOnFailure(HAPI_Result result)
{
    if (result != HAPI_RESULT_SUCCESS)
        throw Failure(result);
}

Asset::Asset(int id)
    : id(id), _info(NULL), _nodeInfo(NULL)
{
}

Asset::Asset(const Asset &asset)
    : id(asset.id)
    , _info(asset._info ? new HAPI_AssetInfo(*asset._info) : NULL)
    , _nodeInfo(asset._nodeInfo ? new HAPI_NodeInfo(*asset._nodeInfo) : NULL)
{
}

Asset::~Asset()
{
    delete this->_info;
    delete this->_nodeInfo;
}

Asset & Asset::operator=(const Asset &asset)
{
    if (this != &asset)
    {
        delete this->_info;
        delete this->_nodeInfo;
        this->id = asset.id;
        this->_info = asset._info ? new HAPI_AssetInfo(*asset._info) : NULL;
        this->_nodeInfo = asset._nodeInfo
                ? new HAPI_NodeInfo(*asset._nodeInfo) : NULL;
    }
    return *this;
}

const HAPI_AssetInfo & Asset::info() const
{
    if (!this->_info)
    {
        this->_info = new HAPI_AssetInfo();
        throwOnFailure(HAPI_GetAssetInfo(this->id, this->_info));
    }
    return *this->_info;
}

const HAPI_NodeInfo & Asset::nodeInfo() const
{
    if (!this->_nodeInfo)
    {
        this->_nodeInfo = new HAPI_NodeInfo();
        throwOnFailure(HAPI_GetNodeInfo(
                           this->info().nodeId, this->_nodeInfo));
    }
    return *this->_nodeInfo;
}

bool Asset::isValid() const
{
    int is_valid = 0;
    try
    {
        // Note that calling info() might fail if the info isn't cached
        // and the asest id is invalid.
        throwOnFailure(HAPI_IsAssetValid(
                           this->id, this->info().validationId, &is_valid));
        return is_valid;
    }
    catch (Failure &failure)
    {
        return false;
    }
}

std::string Asset::name() const
{ return getString(info().nameSH); }

std::string Asset::label() const
{ return getString(info().labelSH); }

std::string Asset::filePath() const
{ return getString(info().filePathSH); }

void Asset::destroyAsset() const
{ throwOnFailure(HAPI_DestroyAsset(this->id)); }

void Asset::cook() const
{ throwOnFailure(HAPI_CookAsset(this->id, NULL)); }

HAPI_TransformEuler Asset::getTransform(
        HAPI_RSTOrder rst_order, HAPI_XYZOrder rot_order) const
{
    HAPI_TransformEuler result;
    throwOnFailure(HAPI_GetAssetTransform(
                       this->id, rst_order, rot_order, &result));
    return result;
}

void Asset::getTransformAsMatrix(float result_matrix[16]) const
{
    HAPI_TransformEuler transform =
            this->getTransform( HAPI_SRT, HAPI_XYZ );
    throwOnFailure(HAPI_ConvertTransformEulerToMatrix(
                       &transform, result_matrix ) );
}


std::string Asset::getInputName( int input, int input_type ) const
{
    HAPI_StringHandle name;
    throwOnFailure(HAPI_GetInputName( this->id,
                                      input, input_type,
                                      &name ) );
    return getString(name);
}


std::vector<Object> Asset::objects() const
{
    std::vector<Object> result;
    for (int object_id=0; object_id < info().objectCount; ++object_id)
        result.push_back(Object(*this, object_id));
    return result;
}


std::vector<Parm> Asset::parms() const
{
    // Get all the parm infos.
    int num_parms = nodeInfo().parmCount;
    std::vector<HAPI_ParmInfo> parm_infos(num_parms);
    throwOnFailure(HAPI_GetParameters(
                       this->info().nodeId, &parm_infos[0], /*start=*/0, num_parms));

    // Get all the parm choice infos.
    std::vector<HAPI_ParmChoiceInfo> parm_choice_infos(
                this->nodeInfo().parmChoiceCount);
    throwOnFailure(HAPI_GetParmChoiceLists(
                       this->info().nodeId, &parm_choice_infos[0], /*start=*/0,
                   this->nodeInfo().parmChoiceCount));

    // Build and return a vector of Parm objects.
    std::vector<Parm> result;
    for (int i=0; i < num_parms; ++i)
        result.push_back(Parm(
                             this->info().nodeId, parm_infos[i], &parm_choice_infos[0]));
    return result;
}

std::map<std::string, Parm> Asset::parmMap() const
{
    std::vector<Parm> parms = this->parms();

    std::map<std::string, Parm> result;
    for (int i=0; i < int(parms.size()); ++i)
        result.insert(std::make_pair(parms[i].name(), parms[i]));

    return result;
}

Object::Object(int asset_id, int object_id)
    : asset(asset_id), id(object_id), _info(NULL)
{}

Object::Object(const Asset &asset, int id)
    : asset(asset), id(id), _info(NULL)
{}

Object::Object(const Object &object)
    : asset(object.asset)
    , id(object.id)
    , _info(object._info ? new HAPI_ObjectInfo(*object._info) : NULL)
{}

Object::~Object()
{ delete this->_info; }

Object & Object::operator=(const Object &object)
{
    if (this != &object)
    {
        delete this->_info;
        asset = object.asset;
        this->id = object.id;
        this->_info = object._info
                ? new HAPI_ObjectInfo(*object._info) : NULL;
    }
    return *this;
}

const HAPI_ObjectInfo & Object::info() const
{
    if (!this->_info)
    {
        this->_info = new HAPI_ObjectInfo();
        throwOnFailure(HAPI_GetObjects(
                           this->asset.id, this->_info, this->id, /*length=*/1));
    }
    return *this->_info;
}

std::vector<Geo> Object::geos() const
{
    std::vector<Geo> result;
    for (int geo_id=0; geo_id < info().geoCount; ++geo_id)
        result.push_back(Geo(*this, geo_id));
    return result;
}

std::string Object::name() const
{ return getString(info().nameSH); }

std::string Object::objectInstancePath() const
{ return getString(info().objectInstancePathSH); }


Geo::Geo(const Object &object, int id)
    : object(object), id(id), _info(NULL)
{}

Geo::Geo(int asset_id, int object_id, int geo_id)
    : object(asset_id, object_id), id(geo_id), _info(NULL)
{}

Geo::Geo(const Geo &geo)
    : object(geo.object)
    , id(geo.id)
    , _info(geo._info ? new HAPI_GeoInfo(*geo._info) : NULL)
{}

Geo::~Geo()
{ delete _info; }

Geo & Geo::operator=(const Geo &geo)
{
    if (this != &geo)
    {
        delete this->_info;
        this->object = geo.object;
        this->id = geo.id;
        this->_info = geo._info ? new HAPI_GeoInfo(*geo._info) : NULL;
    }
    return *this;
}

const HAPI_GeoInfo & Geo::info() const
{
    if (!this->_info)
    {
        this->_info = new HAPI_GeoInfo();
        throwOnFailure(HAPI_GetGeoInfo(
                           this->object.asset.id, this->object.id, this->id, this->_info));
    }
    return *this->_info;
}

std::string Geo::name() const
{ return getString(info().nameSH); }


std::vector<Part> Geo::parts() const
{
    std::vector<Part> result;
    for (int part_id=0; part_id < info().partCount; ++part_id)
        result.push_back(Part(*this, part_id));
    return result;
}


Part::Part(const Geo &geo, int id)
    : geo(geo), id(id), _info(NULL)
{}

Part::Part(int asset_id, int object_id, int geo_id, int part_id)
    : geo(asset_id, object_id, geo_id), id(part_id), _info(NULL)
{}

Part::Part(const Part &part)
    : geo(part.geo)
    , id(part.id)
    , _info(part._info ? new HAPI_PartInfo(*part._info) : NULL)
{}

Part::~Part()
{ delete _info; }

Part & Part::operator=(const Part &part)
{
    if (this != &part)
    {
        delete this->_info;
        this->geo = part.geo;
        this->id = part.id;
        this->_info = part._info ? new HAPI_PartInfo(*part._info) : NULL;
    }
    return *this;
}

const HAPI_PartInfo & Part::info() const
{
    if (!this->_info)
    {
        this->_info = new HAPI_PartInfo();
        throwOnFailure(HAPI_GetPartInfo(
                           this->geo.object.asset.id, this->geo.object.id, this->geo.id,
                           this->id, this->_info));
    }
    return *this->_info;
}

std::string Part::name() const
{ return getString(info().nameSH); }

int Part::numAttribs(HAPI_AttributeOwner attrib_owner) const
{
    switch (attrib_owner)
    {
    case HAPI_ATTROWNER_VERTEX:
        return this->info().vertexAttributeCount;
    case HAPI_ATTROWNER_POINT:
        return this->info().pointAttributeCount;
    case HAPI_ATTROWNER_PRIM:
        return this->info().faceAttributeCount;
    case HAPI_ATTROWNER_DETAIL:
        return this->info().detailAttributeCount;
    case HAPI_ATTROWNER_MAX:
    case HAPI_ATTROWNER_INVALID:
        break;
    }

    return 0;
}

std::vector<std::string> Part::attribNames(HAPI_AttributeOwner attrib_owner) const
{
    int num_attribs = numAttribs(attrib_owner);
    std::vector<int> attrib_names_sh(num_attribs);

    throwOnFailure(HAPI_GetAttributeNames(
                       this->geo.object.asset.id, this->geo.object.id, this->geo.id,
                       this->id, attrib_owner, &attrib_names_sh[0], num_attribs));

    std::vector<std::string> result;
    for (int attrib_index=0; attrib_index < int(attrib_names_sh.size());
         ++attrib_index)
        result.push_back(getString(attrib_names_sh[attrib_index]));
    return result;
}

HAPI_AttributeInfo Part::attribInfo(
        HAPI_AttributeOwner attrib_owner, const char *attrib_name) const
{
    HAPI_AttributeInfo result;
    throwOnFailure(HAPI_GetAttributeInfo(
                       this->geo.object.asset.id, this->geo.object.id, this->geo.id,
                       this->id, attrib_name, attrib_owner, &result));
    return result;
}

float * Part::getNewFloatAttribData(
        HAPI_AttributeInfo &attrib_info, const char *attrib_name,
        int start, int length) const
{
    if (length < 0)
        length = attrib_info.count - start;

    float *result = new float[attrib_info.count * attrib_info.tupleSize];
    throwOnFailure(HAPI_GetAttributeFloatData(
                       this->geo.object.asset.id, this->geo.object.id, this->geo.id,
                       this->id, attrib_name, &attrib_info, result,
                       /*start=*/0, attrib_info.count));
    return result;
}

Parm::Parm()
{ }

Parm::Parm(int node_id, const HAPI_ParmInfo &info,
           HAPI_ParmChoiceInfo *all_choice_infos)
    : node_id(node_id), _info(info)
{
    for (int i=0; i < info.choiceCount; ++i)
        this->choices.push_back(ParmChoice(
                                    all_choice_infos[info.choiceIndex + i]));
}

const HAPI_ParmInfo & Parm::info() const
{ return _info; }

std::string Parm::name() const
{ return getString(_info.nameSH); }

std::string Parm::label() const
{ return getString(_info.labelSH); }

int Parm::getIntValue(int sub_index) const
{
    int result;
    throwOnFailure(HAPI_GetParmIntValues(
                       this->node_id, &result, this->_info.intValuesIndex + sub_index,
                       /*length=*/1));
    return result;
}

float Parm::getFloatValue(int sub_index) const
{
    float result;
    throwOnFailure(HAPI_GetParmFloatValues(
                       this->node_id, &result, this->_info.floatValuesIndex + sub_index,
                       /*length=*/1));
    return result;
}

std::string Parm::getStringValue(int sub_index) const
{
    int string_handle;
    throwOnFailure(HAPI_GetParmStringValues(
                       this->node_id, true, &string_handle,
                       this->_info.stringValuesIndex + sub_index, /*length=*/1));
    return getString(string_handle);
}

void Parm::setIntValue(int sub_index, int value)
{
    throwOnFailure(HAPI_SetParmIntValues(
                       this->node_id, &value, this->_info.intValuesIndex + sub_index,
                       /*length=*/1));
}

void Parm::setFloatValue(int sub_index, float value)
{
    throwOnFailure(HAPI_SetParmFloatValues(
                       this->node_id, &value, this->_info.floatValuesIndex + sub_index,
                       /*length=*/1));
}

void Parm::setStringValue(int sub_index, const char *value)
{
    throwOnFailure(HAPI_SetParmStringValue(
                       this->node_id, value, this->_info.id, sub_index));
}

void Parm::insertMultiparmInstance(int instance_position)
{
    throwOnFailure(HAPI_InsertMultiparmInstance(
                       this->node_id, this->_info.id, instance_position));
}

void Parm::removeMultiparmInstance(int instance_position)
{
    throwOnFailure(HAPI_RemoveMultiparmInstance(
                       this->node_id, this->_info.id, instance_position));
}

ParmChoice::ParmChoice(HAPI_ParmChoiceInfo &info)
    : _info(info)
{}

const HAPI_ParmChoiceInfo & ParmChoice::info() const
{ return _info; }

std::string ParmChoice::label() const
{ return getString(_info.labelSH); }

std::string ParmChoice::value() const
{ return getString(_info.valueSH); }


Engine::Engine() : mInitialized(false)
{

}

Engine::~Engine()
{
    cleanup();
}

bool Engine::initialize( const char * otl_search_path,
                         const char * dso_search_path,
                         bool use_cooking_thread,
                         int cooking_thread_stack_size )
{
    if ( !isInitialize() )
    {
        HAPI_CookOptions cook_options = HAPI_CookOptions_Create();
        mResult = HAPI_Initialize (
                    otl_search_path,
                    dso_search_path,
                    &cook_options,
                    use_cooking_thread,
                    cooking_thread_stack_size );

#if !define( INIT_CHECK_BY_HAPI )
        if (  mResult == HAPI_RESULT_SUCCESS )
            mInitialized = true;
#endif
        return mResult == HAPI_RESULT_SUCCESS;
    }

    // allready initialized
    return true;
}

void Engine::cleanup()
{
    if ( isInitialize() )
    {
        mResult = HAPI_Cleanup();
#if !define( INIT_CHECK_BY_HAPI )
        mInitialized = false;
#endif
    }
}

bool    Engine::isInitialize()
{
#if define( INIT_CHECK_BY_HAPI )
    mResult = HAPI_IsInitialized();
    return mResult == HAPI_RESULT_SUCCESS;
#else
    return mInitialized;
#endif
}

HAPI_Result     Engine::getLastResult()
{
    return mResult;
}

std::string     Engine::getLastError()
{
    int buffer_length;
    HAPI_GetStatusStringBufLength(
                HAPI_STATUS_CALL_RESULT, HAPI_STATUSVERBOSITY_ERRORS, &buffer_length);

    char * buf = new char[ buffer_length ];

    HAPI_GetStatusString(HAPI_STATUS_CALL_RESULT, buf);

    std::string result( buf );
    delete[] buf;

    return result;
}

int Engine::loadAssetLibrary( const char* otl_file )
{
    std::string otl( otl_file );
    int library_id = -1;

    if ( mAssetLib.find( otl ) != mAssetLib.end() )
    {
        library_id = mAssetLib[ otl ];
    }
    else
    {
        mResult = HAPI_LoadAssetLibraryFromFile(
                    otl_file,
                    false,
                    &library_id );

        if ( mResult == HAPI_RESULT_SUCCESS )
        {
            mAssetLib[ otl ] = library_id;
        }
    }

    return library_id;
}

int Engine::getAvailableAssetCount( int library_id )
{
    int count = 0;

    mResult = HAPI_GetAvailableAssetCount( library_id, &count );

    return count;
}

std::string Engine::getAssetName( int library_id, int id )
{
    std::string asset_name;
    int count = getAvailableAssetCount( library_id );

    if ( id >= 0 && id < count )
    {
        HAPI_StringHandle* asset_name_sh = new HAPI_StringHandle[count];

        mResult = HAPI_GetAvailableAssets( library_id, asset_name_sh, count );

        if ( mResult == HAPI_RESULT_SUCCESS )
            asset_name = getString( asset_name_sh[id] );

        delete [] asset_name_sh;
    }

    return asset_name;
}

int Engine::instantiateAsset(  const char* name, bool cook_on_load )
{
    int asset_id = -1;

    mResult = HAPI_InstantiateAsset( name, cook_on_load, &asset_id );

    return asset_id;
}

void Engine::release()
{
    delete this;
    sInstance = nullptr;
}

Engine* Engine::getInstance()
{
    if ( sInstance == nullptr )
    {
        sInstance = new Engine();
    }
    return sInstance;
}

};
