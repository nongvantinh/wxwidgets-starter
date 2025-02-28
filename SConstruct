import os
import subprocess
from SCons.Script import Environment, Command
from SCons.Variables import Variables, EnumVariable
import common

opts = Variables([], ARGUMENTS)

opts.Add(EnumVariable(
    'target',
    'Compilation target',
    'debug',
    allowed_values=('debug', 'release'),
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

user_name = os.getlogin()
mingw_bin_dir = r'C:\ProgramData\mingw64\mingw64\bin'
mingw_include_dir =  r'C:\ProgramData\mingw64\mingw64\x86_64-w64-mingw32\include'
mingw_lib_dir =  r'C:\ProgramData\mingw64\mingw64\x86_64-w64-mingw32\lib'
temp_dir = f"C:/Users/{user_name}/AppData/Local/Temp"
project_dir = os.path.join(os.getcwd(), 'extract-bitlocker-key')

def build_wx(target, wx_dir):
    """Build wxWidgets from source using mingw32-make."""
    makefile_dir = os.path.join(wx_dir, 'build', 'msw')
    mingw_make_command = r'C:\ProgramData\mingw64\mingw64\bin\mingw32-make.exe'
    
    makefile_gcc_path = os.path.join(makefile_dir, 'makefile.gcc')
    if not os.path.isfile(makefile_gcc_path):
        raise FileNotFoundError(f"makefile.gcc not found in {makefile_dir}")
    
    original_dir = os.getcwd()
    
    os.chdir(makefile_dir)
    
    try:
        subprocess.run([mingw_make_command, '-f', 'makefile.gcc', 'SHARED=0', 'UNICODE=1', f'BUILD={target}', 'clean'], check=True)
        subprocess.run([mingw_make_command, '-f', 'makefile.gcc', 'SHARED=0', 'UNICODE=1', f'BUILD={target}', 'setup_h'], check=True)
        subprocess.run([mingw_make_command, '-f', 'makefile.gcc', 'SHARED=0', 'UNICODE=1', f'BUILD={target}', '-j16'], check=True)
    finally:
        os.chdir(original_dir)

user_name = os.getlogin()
temp_dir = f"C:/Users/{user_name}/AppData/Local/Temp"

env = Environment(tools=['mingw'])
opts.Update(env)
Help(opts.GenerateHelpText(env))

wx_dir = os.path.splitext(env['wx_source'])[0]
if env['build_wx'] == 'yes':
    wx_source_zip = env['wx_source']
    wx_dir = common.extract_zip(wx_source_zip)
    build_wx(env['target'], wx_dir)

sources = [str(f) for f in Glob('extract-bitlocker-key/*.cpp')]
wx_lib_dir = os.path.join(wx_dir, 'lib', 'gcc_lib')
wx_include_dir = os.path.join(wx_dir, 'include')
wx_setup_dir = os.path.join(wx_lib_dir, 'mswu')
windows_libraries = [
    'crypt32', 'kernel32', 'user32', 'advapi32', 'shell32', 'uuid', 'winspool', 'oleacc',
    'gdi32', 'ole32', 'oleaut32', 'shlwapi','version', 'comctl32', 'uxtheme',
]
wx_libraries = []
if(env['target'] == 'debug'):
    wx_setup_dir = os.path.join(wx_lib_dir, 'mswud')
    wx_libraries = [ 'wxmsw32ud_core', 'wxexpatd', 'wxjpegd', 'wxmsw32ud_adv', 'wxmsw32ud_aui',
                    'wxmsw32ud_gl', 'wxmsw32ud_html', 'wxmsw32ud_media', 'wxmsw32ud_propgrid',
                    'wxmsw32ud_ribbon', 'wxmsw32ud_richtext', 'wxmsw32ud_stc', 'wxmsw32ud_webview',
                    'wxmsw32ud_xrc', 'wxpngd', 'wxregexud', 'wxscintillad', 'wxtiffd', 'wxzlibd',
                    'wxbase32ud', 'wxbase32ud_net', 'wxbase32ud_xml']
elif(env['target'] == 'release'):
    wx_setup_dir = os.path.join(wx_lib_dir, 'mswu')
    wx_libraries = [
        'wxmsw32u_core', 'wxmsw32u_adv', 'wxmsw32u_aui', 'wxmsw32u_html', 'wxmsw32u_xrc',
        'wxbase32u', 'wxbase32u_net', 'wxbase32u_xml', 'wxexpat', 'wxjpeg', 'wxpng',
        'wxregexu', 'wxscintilla', 'wxtiff', 'wxzlib'
    ]

env.Append(CXXFLAGS=['-DUNICODE', '-D_UNICODE'])
env.Append(CXXFLAGS = ['-std=c++17'])

env.Append(CPPPATH = [project_dir, wx_include_dir, wx_setup_dir, mingw_include_dir])
env.Append(LIBPATH = [wx_lib_dir, mingw_lib_dir])
# Cannot use Append on this since this key doesn't exist by default
env["LIBS"] = [wx_libraries, windows_libraries]

env.Append(LINKFLAGS=['-mwindows'])

env.Program(target='ExtractBitlocker.exe', source=sources)
