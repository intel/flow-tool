# flow-tool

Command line tool, built using plugin architecture, written in C++, to assist software package deployment, for an end-to-end use case enablement on target Linux system.

This Tool, on target platform does following
  * Discover system
  * Run gap analysis against golden manifest to identify missing components on target system
  * Reverts back if requirements not met.
  * Generate customized manifest for target system
  * Execute manifest tasks in sequence to setup end to end use case.
  * Rolls back in case of failure

golden manifest: Json file, containing list of tasks [software packages to install / scripts/commands to execute] on target system, identified on reference system.

# Folder Hierarchy:
  * readme.md [this file]
  * buildScript
    * build.sh - builds all projects under this repo.
    * config
      * configuration files.
  * bundleScript
    * scripts to bundle
  * src 
    * framework  
    * common
    * plugin
      * common
      * plugin1
      * plugin2

# Build environment setup 
  * refer to scripts under "devSetup" folder for build environment setup
  
## Additional installs [if applicable]
sudo apt install -y git build-essential

# Build projects

  * refer to buildScript/build.sh master script to build framework and all plugins.
  
  or 
  
  cmake -S .. -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build

  do this for framework and for each plugin folder [after changing current directory.]
  * src/framework/buildScript
  * src/plugins/*plugin folder*/buildScript

# Out - Folder Hierarchy
  *	flow-tool_linux_x86_64 [executable]
  *	lib/ [folder containing tool plugins]
    * discovery plugin [dynamic library]
    * analysis plugin [dynamic library]
    * deploy plugin [dynamic library]
  *	config/ [folder contains configuration files][JSON]
    *	What to discover?, rules to follow for analysis, where is deployment manifest file path? [JSON]
  * schema/ [folder contains JSON schema - rules][JSON]

# Usage


command interaction:
 <tool_name> <tool options> --<plugin_name> <plugin_param>
 
  to get version of tool
./flow-tool_linux_x86_64 --version

 to get tool info
./flow-tool_linux_x86_64 --info

to enable more logs
./flow-tool_linux_x86_64 --verbose
 
list all plugins:
 ./flow-tool_linux_x86_64 --verbose --list
 
APIs supported by plugins refer to plugin user guide.
 
to deploy package changes
./flow-tool_linux_x86_64 --verbose --deploy "{\"api\": \"deploy\", \"manifest\": \"TARGET_FOLDER/*.manifest.json\"}"
 
 to peek into actions
./flow-tool_linux_x86_64 --verbose --deploy "{\"api\": \"prepare\", \"manifest\": \"TARGET_FOLDER/*.manifest.json\"}"

#validate a json file with schema
./flow-tool_linux_x86_64 --verbose --deploy "{\"api\": \"validate\", \"manifest\":\"pkg-manifest.json\"}"

to collect the target platform information and generate the file locally
./flow-tool_linux_x86_64 --verbose --discovery "{\"api\": \"collect\", \"nameonly\":true}"
./flow-tool_linux_x86_64 --verbose --discovery "{\"api\": \"collect\", \"nameonly\":false }"
