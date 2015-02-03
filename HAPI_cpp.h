#ifndef HAPI_CPP_H
#define HAPI_CPP_H

#include <HAPI/HAPI.h>
#include <string>
#include <vector>
#include <map>

namespace hapi
{

std::string getString( int string_handle );

//----------------------------------------------------------------------------
// Common error handling:

// Instances of this class are thrown whenever an underlying HAPI function
// call fails.
class Failure
{
public:
    Failure(HAPI_Result result)
    : result(result)
    {}

    // Retrieve details about the last non-successful HAPI function call.
    // You would typically call this method after catching a Failure exception,
    // but it can be called as a static method after calling a C HAPI function.
    static std::string lastErrorMessage()
    {
    int buffer_length;
    HAPI_GetStatusStringBufLength(
            HAPI_STATUS_CALL_RESULT, HAPI_STATUSVERBOSITY_ERRORS, &buffer_length);

    char * buf = new char[ buffer_length ];

    HAPI_GetStatusString(HAPI_STATUS_CALL_RESULT, buf);
        std::string result(buf);
    return result;
    }

    HAPI_Result result;
};


//----------------------------------------------------------------------------
// Classes:

class Object;
class Parm;

class Asset
{
public:
    Asset(int id);
    Asset(const Asset &asset);
    ~Asset();

    Asset &operator=(const Asset &asset);

    const HAPI_AssetInfo &info() const;
    const HAPI_NodeInfo &nodeInfo() const;

    std::vector<Object> objects() const;
    std::vector<Parm> parms() const;
    std::map<std::string, Parm> parmMap() const;

    bool isValid() const;

    std::string name() const;
    std::string label() const;
    std::string filePath() const;
    void destroyAsset() const;
    void cook() const;
    HAPI_TransformEuler getTransform(
        HAPI_RSTOrder rst_order, HAPI_XYZOrder rot_order) const;
    void getTransformAsMatrix(float result_matrix[16]) const;

    std::string getInputName( int input, int input_type = HAPI_INPUT_GEOMETRY ) const;
    int id;
private:
    mutable HAPI_AssetInfo *_info;
    mutable HAPI_NodeInfo *_nodeInfo;
};

class Geo;

class Object
{
public:
    Object(int asset_id, int object_id);
    Object(const Asset &asset, int id);
    Object(const Object &object);

    ~Object();

    Object &operator=(const Object &object);
    const HAPI_ObjectInfo &info() const;
    std::vector<Geo> geos() const;
    std::string name() const;
    std::string objectInstancePath() const;
    Asset asset;
    int id;
private:
    mutable HAPI_ObjectInfo *_info;
};

class Part;

class Geo
{
public:
    Geo(const Object &object, int id);
    Geo(int asset_id, int object_id, int geo_id);
    Geo(const Geo &geo);
    ~Geo();
    Geo &operator=(const Geo &geo);
    const HAPI_GeoInfo &info() const;
    std::string name() const;
    std::vector<Part> parts() const;
    Object object;
    int id;
private:
    mutable HAPI_GeoInfo *_info;
};

class Part
{
public:
    Part(const Geo &geo, int id);
    Part(int asset_id, int object_id, int geo_id, int part_id);
    Part(const Part &part);
    ~Part();

    Part &operator=(const Part &part);
    const HAPI_PartInfo &info() const;
    std::string name() const;
    int numAttribs(HAPI_AttributeOwner attrib_owner) const;
    std::vector<std::string> attribNames(HAPI_AttributeOwner attrib_owner) const;
    HAPI_AttributeInfo attribInfo(
    HAPI_AttributeOwner attrib_owner, const char *attrib_name) const;
    float *getNewFloatAttribData(
    HAPI_AttributeInfo &attrib_info, const char *attrib_name,
    int start=0, int length=-1) const;
    Geo geo;
    int id;
private:
    mutable HAPI_PartInfo *_info;
};

class ParmChoice;

class Parm
{
public:
    // This constructor is required only for std::map::operator[] for the case
    // where a Parm object does not exist in the map.
    Parm();
    Parm(int node_id, const HAPI_ParmInfo &info,
    HAPI_ParmChoiceInfo *all_choice_infos);

    const HAPI_ParmInfo &info() const;
    std::string name() const;
    std::string label() const;
    int getIntValue(int sub_index) const;
    float getFloatValue(int sub_index) const;
    std::string getStringValue(int sub_index) const;
    void setIntValue(int sub_index, int value);
    void setFloatValue(int sub_index, float value);
    void setStringValue(int sub_index, const char *value);
    void insertMultiparmInstance(int instance_position);
    void removeMultiparmInstance(int instance_position);
    int node_id;
    std::vector<ParmChoice> choices;
private:
    HAPI_ParmInfo _info;
};

class ParmChoice
{
public:
    ParmChoice(HAPI_ParmChoiceInfo &info);
    const HAPI_ParmChoiceInfo &info() const;
    std::string label() const;
    std::string value() const;
private:
    HAPI_ParmChoiceInfo _info;
};

class Engine
{
private:
    Engine();
    ~Engine();
public:

    bool        initialize( const char * otl_search_path,
                            const char * dso_search_path,
                            bool use_cooking_thread,
                            int cooking_thread_stack_size = -1 );
    void        cleanup();

    bool        isInitialize();
    HAPI_Result getLastResult();
    std::string getLastError();

    int         loadAssetLibrary( const char* otl_file );
    int         getAvailableAssetCount( int library_id );
    std::string getAssetName( int library_id, int id );
    int         instantiateAsset(  const char* name, bool cook_on_load = true );

    int         getAssetNames();
    std::string getAssetName( int id );


    void        release();
    static Engine* getInstance();

private:
    static Engine*                  sInstance;

    bool                            mInitialized;
    HAPI_Result                     mResult;
    std::map<std::string, int>      mAssetLib;
    std::vector<std::string>        mAssetNames;
};

}

#endif // HAPI_CPP_H
