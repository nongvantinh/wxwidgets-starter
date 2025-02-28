import os
from SCons.Variables import Variables, EnumVariable
import config

opts = Variables([], ARGUMENTS)

opts.Add(EnumVariable(
    'target',
    'Compilation target',
    'debug',
    allowed_values=('debug', 'release'),
    ignorecase=2
))

opts.Add(EnumVariable(
    'vsproj',
    'Generate Visual Studio project files',
    'no',
    allowed_values=('yes', 'no'),
    ignorecase=2
))

opts.Add(EnumVariable(
    'build_wx',
    'Compile wxWidgets before building the project',
    'no',
    allowed_values=('yes', 'no'),
    ignorecase=2
))

opts.Add('wx_source', 'Path to wxWidgets.zip source code', 'wxWidgets-3.2.6.zip')

env = Environment(tools=['default', 'msvs', 'mingw'])
opts.Update(env)
Help(opts.GenerateHelpText(env))

print("Generating Visual Studio project files: {} and {}".format(os.getenv("VCINSTALLDIR"), os.getenv("VCTOOLSINSTALLDIR")))
solution_configs = [
    ('debug', 'x86'),
    ('debug', 'x64'),
    ('release', 'x86'),
    ('release', 'x64'),
]

msvs_variants = [f"{config[0]}|{config[1]}" for config in solution_configs]
Export("solution_configs", "msvs_variants", "env")

projects_list = []

SConscript('extract-bitlocker-key/SConscript')

if env['vsproj'] == 'yes':
    Import('extract_bitlocker_key_project')
    projects_list.append(extract_bitlocker_key_project)

    env.MSVSSolution(
        target=config.SOLUTION_NAME + env['MSVSSOLUTIONSUFFIX'],
        projects=projects_list,
        variant=msvs_variants,
    )
