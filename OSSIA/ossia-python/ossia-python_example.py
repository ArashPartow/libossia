#! /usr/bin/env python
# -*- coding: utf-8 -*-

"""
This file illustrates how to use the python binding of Ossia library.
This is a low level approach to integrate Ossia architecture into a python program.
For higher level features use pyossia module.
"""

import ossia_python as ossia
import time

print("OSSIA LIBRARY EXAMPLE")

### LOCAL DEVICE SETUP
# create a device for this python program
local_device = ossia.LocalDevice("newDevice")

print("\nlocal device name: " + local_device.name)

# enable OSCQuery communication for our device without messages logging
local_device.create_oscquery_server(3456, 5678, False)

# enable OSC communication for that device without messages logging
local_device.create_osc_server("127.0.0.1", 9997, 9996, False)

# enable MIDI communication for that device
### TODO : enable MIDI communication

# list all devices on the network
print('\nSCAN FOR OSCQUERY DEVICES\n')
for data in ossia.list_oscquery_devices():
  print(data.name + ": host = " + data.host + ", port = " + str(data.port))

# create a node, create a boolean parameter and initialize it
bool_node = local_device.add_node("/test/special/bool")
bool_parameter = bool_node.create_parameter(ossia.ValueType.Bool)
bool_parameter.access_mode = ossia.AccessMode.Get
bool_parameter.value = True
### TODO : bool_parameter.defaultvalue = True

# create a node, create an integer parameter and initialize it
int_node = local_device.add_node("/test/numeric/int")
int_parameter = int_node.create_parameter(ossia.ValueType.Int)

int_parameter.access_mode = ossia.AccessMode.Set
int_parameter.bounding_mode = ossia.BoundingMode.Clip
int_parameter.value = 9
int_parameter.make_domain(-10, 10)
int_parameter.apply_domain()
int_parameter.repetition_filter = ossia.RepetitionFilter.Off
### TODO : int_parameter.defaultvalue = -3

# create a node, create a float parameter, set its properties and initialize it
float_node = local_device.add_node("/test/numeric/float")
float_parameter = float_node.create_parameter(ossia.ValueType.Float)
float_parameter.access_mode = ossia.AccessMode.Bi
float_parameter.bounding_mode = ossia.BoundingMode.Fold
float_parameter.value = 1.5
float_parameter.make_domain(-2.0, 2.0)
float_parameter.apply_domain()
### TODO : float_parameter.defaultvalue = 0.123456789

# create a node, create a char parameter and initialize it
char_node = local_device.add_node("/test/misc/char")
char_parameter = char_node.create_parameter(ossia.ValueType.Char)
char_parameter.value = 'a'
### TODO : char_parameter.defaultvalue = chr(69)

# create a node, create a string parameter and initialize it
string_node = local_device.add_node("/test/misc/string")
string_parameter = string_node.create_parameter(ossia.ValueType.String)
string_parameter.value = "hello world !"
### TODO : string_parameter.defaultvalue = ['init value']
### TODO : string_parameter.make_domain(['once', 'loop', 'ping-pong'])
#string_parameter.apply_domain()

# create a node, create a 3 floats vector parameter and initialize it
vec3f_node = local_device.add_node("/test/numeric/vec3f")
vec3f_parameter = vec3f_node.create_parameter(ossia.ValueType.Vec3f)
vec3f_parameter.value = [0, 146, 207]
### TODO : vec3f_parameter.defaultvalue = [0, 146, 207]
### TODO : vec3f_parameter.make_domain([0, 255])
#vec3f_parameter.apply_domain()

# create a node, create a list parameter and initialize it
list_node = local_device.add_node("/test/misc/list")
list_parameter = list_node.create_parameter(ossia.ValueType.List)
list_parameter.value = [44100, "test.wav", 0.9]
### TODO : list_parameter.defaultvalue = [44100, "ossia.wav", 0.9]

# attach a callback function to the boolean parameter
def bool_value_callback(v):
  print(v)
bool_parameter.add_callback(bool_value_callback)
### TODO : v must be a Bool, not a ossia_python.Value

# attach a callback function to the integer parameter
def int_value_callback(v):
  print(v)
int_parameter.add_callback(int_value_callback)
### TODO : v must be an Int, not a ossia_python.Value

# attach a callback function to the float parameter
def float_value_callback(v):
  print(v)
float_parameter.add_callback(float_value_callback)
### TODO : v must be a Float, not a ossia_python.Value

# attach a callback function to the char parameter
def char_value_callback(v):
  print(v)
char_parameter.add_callback(char_value_callback)
### TODO : v must be a Char, not a ossia_python.Value

# attach a callback function to the string parameter
def string_value_callback(v):
  print(v)
string_parameter.add_callback(string_value_callback)
### TODO : v must be a String, not a ossia_python.Value

# attach a callback function to the 3 floats vector parameter
def vec3f_value_callback(v):
  print(v)
vec3f_parameter.add_callback(vec3f_value_callback)
### TODO : v must be a Tuple, not a ossia_python.Value

# attach a callback function to the list parameter
def list_value_callback(v):
  print(v)
list_parameter.add_callback(list_value_callback)
### TODO : v must be a List, not a ossia_python.Value


### LOCAL DEVICE EXPLORATION

### TODO : MAYBE THIS FUNCTION COULD BE DONE IN C++ ???
### TODO : IT CAN BE GET_NODES, AND GET_PARAMS WITH A DEPTH ATTRIBUTE
# DEPTH = 0 => ALL LEVELS
# a function to iterate on node's tree recursively
def iterate_on_children(node):

  for child in node.children():
    print('-------------------------------------')
    if child.parameter:
      print('PARAMETER -> ' + str(child))
      print(str(child.parameter))
      print(str(child.parameter.value_type))
      print(str(child.parameter.access_mode))
      print(str(child.parameter.repetition_filter))
      print('callbacks : ' + str(child.parameter.callback_count))
      # displaying the domain bounds for the float parameter crashes ... ???
      if child.parameter.have_domain():
        print('--- -domain- ---')
        print('min : ' + str(child.parameter.domain.min) + ' / max : ' + str(child.parameter.domain.max), str(child.parameter.bounding_mode))
      else:
        print('--- -no domain- ---')
    else:
      print()
      print('\nNODE -> ' + str(child))
            #print(getattr(float_parameter, prop))
    iterate_on_children(child)


# iterate on our device
print("\nLOCAL DEVICE NAMESPACE")
iterate_on_children(local_device.root_node)


### REMOTE OSCQUERY DEVICE FEATURES

# try to connect to a remote device using OSCQuery protocol
remote_oscquery_device = ossia.OSCQueryDevice("remoteOSCQueryDevice", "ws://127.0.0.1:5678", 9998)

# update the remote OSCQuery device namespace
remote_oscquery_device.update()

# iterate on remote OSCQuery device namespace
print("\nREMOTE OSCQUERY DEVICE NAMESPACE")
iterate_on_children(remote_oscquery_device.root_node)


### REMOTE OSC DEVICE FEATURES

# try to connect to a remote device using OSC protocol
remote_osc_device = ossia.OSCDevice("remoteOSCDevice", "127.0.0.1", 10000, 10001)

# create a remote node, create a boolean parameter and initialize it
remote_bool_node = remote_osc_device.add_node("/test/special/bool")
remote_bool_parameter = remote_bool_node.create_parameter(ossia.ValueType.Bool)
remote_bool_parameter.access_mode = ossia.AccessMode.Get
remote_bool_parameter.value = True
### TODO : remote_bool_parameter.defaultvalue = True

# learn namespace from message sent by the remote OSC device
remote_osc_device.learning = True

# wait 10 seconds 
print("\nLEARNING FROM INCOMING OSC MESSAGES ...")
count = 0
while count < 10:
  time.sleep(1)
  count += 1
  print(str(10 - count) + "s")

remote_osc_device.learning = False

# iterate on remote OSC device namespace
print("\nREMOTE OSC DEVICE NAMESPACE")
iterate_on_children(remote_osc_device.root_node)


### REMOTE MIDI DEVICE FEATURES
'''
# try to connect to a remote device using MIDI protocol
remote_midi_device = ossia.MidiDevice("remoteMidiDevice")

# load the remote MIDI device map
remote_midi_device.load("/path/to/map/file")

# iterate on remote MIDI device namespace
print("\nREMOTE MIDI DEVICE NAMESPACE")
iterate_on_children(remote_midi_device.root_node)
'''

# MAIN LOOP
# wait and use Ossia Score to change the value remotely
while True:
  time.sleep(0.1)