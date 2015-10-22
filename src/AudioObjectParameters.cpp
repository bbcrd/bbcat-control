
#include <stdlib.h>
#include <string.h>

#define DEBUG_LEVEL 1
#include "AudioObjectParameters.h"

BBC_AUDIOTOOLBOX_START

// order taken from Parameter_t enumeration
const char *AudioObjectParameters::ParameterNames[Parameter_count] =
{
    "channel",

    "position",

    "gain",

    "width",
    "depth",
    "height",

    "diffuseness",
    "delay",

    "importance",
    "dialogue",

    "channellock",
    "interact",
    "interpolate",
    "interpolationtime",
    "onscreen",

    "othervalues",
};

AudioObjectParameters::AudioObjectParameters() : setbitmap(0)
{
  InitialiseToDefaults();
}

AudioObjectParameters::AudioObjectParameters(const AudioObjectParameters& obj) : setbitmap(0)
{
  InitialiseToDefaults();
  operator = (obj);
}

#if ENABLE_JSON
AudioObjectParameters::AudioObjectParameters(const json_spirit::mObject& obj) : setbitmap(0)
{
  InitialiseToDefaults();
  operator = (obj);
}
#endif

AudioObjectParameters::~AudioObjectParameters()
{
}

/*--------------------------------------------------------------------------------*/
/** Initialise all parameters to defaults
 */
/*--------------------------------------------------------------------------------*/
void AudioObjectParameters::InitialiseToDefaults()
{
  // reset all values to zero
  memset(&values, 0, sizeof(values));

  // explicitly reset those parameters whose reset values are not zero
  ResetPosition();
  ResetGain();
  ResetOtherValues();
}

/*--------------------------------------------------------------------------------*/
/** Assignment operator
 */
/*--------------------------------------------------------------------------------*/
AudioObjectParameters& AudioObjectParameters::operator = (const AudioObjectParameters& obj)
{
  // do not copy oneself
  if (&obj != this)
  {
    position    = obj.position;
    values      = obj.values;
    othervalues = obj.othervalues;
    setbitmap   = obj.setbitmap;
  }

  return *this;
}

#if ENABLE_JSON
/*--------------------------------------------------------------------------------*/
/** Assignment operator
 */
/*--------------------------------------------------------------------------------*/
AudioObjectParameters& AudioObjectParameters::FromJSON(const json_spirit::mObject& obj, bool reset)
{
  Position     pval;
  ParameterSet sval;
  double       fval;
  int          ival;
  bool         bval;
  
  Set<>(Parameter_channel, values.channel, ival, obj, reset, 0U, &Limit0);
  Set<>(Parameter_position, position, pval, obj, reset);
  Set<>(Parameter_gain, values.gain, fval, obj, reset, 1.0);
  Set<>(Parameter_width, values.width, fval, obj, reset, 0.0, &Limit0);
  Set<>(Parameter_depth, values.depth, fval, obj, reset, 0.0, &Limit0);
  Set<>(Parameter_height, values.height, fval, obj, reset, 0.0, &Limit0);
  Set<>(Parameter_diffuseness, values.diffuseness, fval, obj, reset, 0.0, &Limit0to1);
  Set<>(Parameter_delay, values.delay, fval, obj, reset, 0.0, &Limit0);
  Set<>(Parameter_importance, values.importance, ival, obj, reset, (uint8_t)0, &LimitImportance);
  Set<>(Parameter_dialogue, values.dialogue, ival, obj, reset, (uint8_t)0, &LimitDialogue);
  Set<>(Parameter_channellock, values.channellock, bval, obj, reset);
  Set<>(Parameter_interact, values.interact, bval, obj, reset);
  Set<>(Parameter_interpolate, values.interpolate, bval, obj, reset);
  Set<>(Parameter_interpolationtime, values.interpolationtime, fval, obj, reset);
  Set<>(Parameter_onscreen, values.onscreen, bval, obj, reset);
  Set<>(Parameter_othervalues, othervalues, sval, obj, reset);
  
  return *this;
}
#endif

/*--------------------------------------------------------------------------------*/
/** Comparison operator
 */
/*--------------------------------------------------------------------------------*/
bool AudioObjectParameters::operator == (const AudioObjectParameters& obj) const
{
  return ((position == obj.position) &&
          (memcmp(&values, &obj.values, sizeof(obj.values)) == 0) &&
          (othervalues == obj.othervalues));
}

/*--------------------------------------------------------------------------------*/
/** Transform this object's position and return new copy
 */
/*--------------------------------------------------------------------------------*/
AudioObjectParameters operator * (const AudioObjectParameters& obj, const PositionTransform& transform)
{
  AudioObjectParameters res = obj;
  res.SetPosition(obj.GetPosition() * transform);
  return res;
}

/*--------------------------------------------------------------------------------*/
/** Transform this object's position
 */
/*--------------------------------------------------------------------------------*/
AudioObjectParameters& AudioObjectParameters::operator *= (const PositionTransform& transform)
{
  Position centre = GetPosition(), corner = centre;

  SetPosition(centre * transform);
 
  if (centre.polar)
  {
    corner.pos.az += GetWidth();
    corner.pos.el += GetHeight();
    corner.pos.d  += GetDepth();
  }
  else
  {
    corner.pos.x  += GetWidth();
    corner.pos.y  += GetHeight();
    corner.pos.z  += GetDepth();
  }

  corner = (corner * transform) - centre;

  if (centre.polar)
  {
    SetWidth(corner.pos.az);
    SetHeight(corner.pos.el);
    SetDepth(corner.pos.d);
  }
  else
  {
    SetWidth(corner.pos.x);
    SetHeight(corner.pos.y);
    SetDepth(corner.pos.z);
  }
  
  return *this;
}

/*--------------------------------------------------------------------------------*/
/** Convert all parameters into text and store them in a ParameterSet object 
 *
 * @param set ParameterSet object to receive parameters
 */
/*--------------------------------------------------------------------------------*/
void AudioObjectParameters::GetAll(ParameterSet& set, bool force) const
{
  Get<>(Parameter_channel, values.channel, set, force);
  if (force || IsSet(Parameter_position)) position.SetParameters(set, ParameterNames[Parameter_position]);
  Get<>(Parameter_gain, values.gain, set, force);
  Get<>(Parameter_width, values.width, set, force);
  Get<>(Parameter_depth, values.depth, set, force);
  Get<>(Parameter_height, values.height, set, force);
  Get<>(Parameter_diffuseness, values.diffuseness, set, force);
  Get<>(Parameter_delay, values.delay, set, force);
  Get<>(Parameter_importance, values.importance, set, force);
  Get<>(Parameter_dialogue, values.dialogue, set, force);
  Get<>(Parameter_channellock, values.channellock, set, force);
  Get<>(Parameter_interact, values.interact, set, force);
  Get<>(Parameter_interpolate, values.interpolate, set, force);
  Get<>(Parameter_interpolationtime, values.interpolationtime, set, force);
  Get<>(Parameter_onscreen, values.onscreen, set, force);
  Get<>(Parameter_othervalues, othervalues, set, force);
}

/*--------------------------------------------------------------------------------*/
/** Convert parameters to a string
 */
/*--------------------------------------------------------------------------------*/
std::string AudioObjectParameters::ToString(bool pretty) const
{
  ParameterSet params;

  GetAll(params);

  return params.ToString(pretty);
}

#if ENABLE_JSON
/*--------------------------------------------------------------------------------*/
/** Convert parameters to a JSON object
 */
/*--------------------------------------------------------------------------------*/
void AudioObjectParameters::ToJSON(json_spirit::mObject& obj, bool force) const
{
  Get<>(Parameter_channel, (int)values.channel, obj, force);
  GetEx<>(Parameter_position, position, obj, force);
  Get<>(Parameter_gain, values.gain, obj, force);
  Get<>(Parameter_width, values.width, obj, force);
  Get<>(Parameter_depth, values.depth, obj, force);
  Get<>(Parameter_height, values.height, obj, force);
  Get<>(Parameter_diffuseness, values.diffuseness, obj, force);
  Get<>(Parameter_delay, values.delay, obj, force);
  Get<>(Parameter_importance, (int)values.importance, obj, force);
  Get<>(Parameter_dialogue, (int)values.dialogue, obj, force);
  Get<>(Parameter_channellock, values.channellock, obj, force);
  Get<>(Parameter_interact, values.interact, obj, force);
  Get<>(Parameter_interpolate, values.interpolate, obj, force);
  Get<>(Parameter_interpolationtime, values.interpolationtime, obj, force);
  Get<>(Parameter_onscreen, values.onscreen, obj, force);
  GetEx<>(Parameter_othervalues, othervalues, obj, force);

  DEBUG2(("JSON: %s", json_spirit::write(obj, json_spirit::pretty_print).c_str()));
}
#endif

/*----------------------------------------------------------------------------------------------------*/

AudioObjectParameters::Modifier::Modifier(const Modifier& obj) : RefCountedObject()
{
  operator = (obj);
}

/*--------------------------------------------------------------------------------*/
/** Assignment operator
 */
/*--------------------------------------------------------------------------------*/
AudioObjectParameters::Modifier& AudioObjectParameters::Modifier::operator = (const Modifier& obj)
{
  if (&obj != this)
  {
    rotation = obj.rotation;
    position = obj.position;
    gain     = obj.gain;
    scale    = obj.scale;
  }
  return *this;
}

/*--------------------------------------------------------------------------------*/
/** Comparison operator
 */
/*--------------------------------------------------------------------------------*/
bool AudioObjectParameters::Modifier::operator == (const Modifier& obj) const
{
  return ((rotation == obj.rotation) &&
          (position == obj.position) &&
          (gain     == obj.gain)     &&
          (scale    == obj.scale));
}

#if ENABLE_JSON
/*--------------------------------------------------------------------------------*/
/** Assignment operator
 */
/*--------------------------------------------------------------------------------*/
AudioObjectParameters::Modifier& AudioObjectParameters::Modifier::FromJSON(const json_spirit::mObject& obj)
{
  INamedParameter *parameters[] =
  {
    &rotation,
    &position,
    &gain,
    &scale,
  };
  bbcat::FromJSON(obj, parameters, NUMBEROF(parameters));
  return *this;
}

/*--------------------------------------------------------------------------------*/
/** Convert parameters to a JSON object
 */
/*--------------------------------------------------------------------------------*/
void AudioObjectParameters::Modifier::ToJSON(json_spirit::mObject& obj) const
{
  const INamedParameter *parameters[] =
  {
    &rotation,
    &position,
    &gain,
    &scale,
  };
  bbcat::ToJSON(parameters, NUMBEROF(parameters), obj);
}
#endif

/*--------------------------------------------------------------------------------*/
/** Specific modifications
 */
/*--------------------------------------------------------------------------------*/
void AudioObjectParameters::Modifier::Modify(AudioObjectParameters& parameters, const AudioObject *object) const
{
  UNUSED_PARAMETER(parameters);
  UNUSED_PARAMETER(object);
}

/*----------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
/** Modify this object's parameters using a single modifier
 */
/*--------------------------------------------------------------------------------*/
AudioObjectParameters& AudioObjectParameters::Modify(const Modifier& modifier, const AudioObject *object)
{
  if (modifier.rotation.IsSet())
  {
    SetPosition(GetPosition() * modifier.rotation.Get());
    Position size(GetWidth(), GetHeight(), GetDepth());
    size *= modifier.rotation.Get();
    SetWidth(size.pos.x);
    SetHeight(size.pos.y);
    SetDepth(size.pos.z);
  }
  if (modifier.position.IsSet()) SetPosition(GetPosition() + modifier.position.Get());
  if (modifier.scale.IsSet())
  {
    SetPosition(GetPosition() * modifier.scale.Get());
    Position size(GetWidth(), GetHeight(), GetDepth());
    size *= modifier.scale.Get();
    SetWidth(size.pos.x);
    SetHeight(size.pos.y);
    SetDepth(size.pos.z);
  }
  if (modifier.gain.IsSet()) SetGain(GetGain() * modifier.gain.Get());

  // apply specific modifications (from derived classes)
  modifier.Modify(*this, object);

  return *this;
}

/*--------------------------------------------------------------------------------*/
/** Modify this object's parameters using a list of modifiers
 */
/*--------------------------------------------------------------------------------*/
AudioObjectParameters& AudioObjectParameters::Modify(const Modifier::LIST& list, const AudioObject *object)
{
  uint_t i;

  for (i = 0; i < list.size(); i++) Modify(*list[i].Obj(), object);

  return *this;
}

BBC_AUDIOTOOLBOX_END
