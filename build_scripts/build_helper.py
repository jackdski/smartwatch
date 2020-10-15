import os
import sys
import json

PROJECT_BASE_DIR = "jd_smartwatch"


class BuildHelper:
    def __init__(self):
        self.build_dir = "build"
        self.target = "jd_smartwatch"
        self.build_type = "Debug"

        self.cmds = [
            'build',
            'build_debug',
            'build_release'
            'build_with_bootloader',
            'build_secure_bootloader',
            'flash',
            'flash_bootloader',
            'pkg_dfu',
            'pkg_bl_dfu',
            'clean',
            'logger',
            'erase_flash',
            'make_venv',
            'analyze_map'
        ]

        # start venv
        self.start_venv()

        # set Env Vars based on config.json
        self.read_config()
        print(os.environ['xIC'])

    def read_config(self):
        with open('../config.json') as f:
            config_data = json.load(f)
            self.target = config_data['name']
            self.build_type = config_data['build_type']
            self.build_dir = os.getcwd().replace(os.getcwd().split('/')[-1], '') + '/' + config_data['build_dir'] + '_' + self.build_type.lower()
            os.environ['xTARGET'] = self.target
            version_data = config_data['version'].split('.')
            os.environ['xVERSION_MAJOR'] = version_data[0]
            os.environ['xVERSION_MINOR'] = version_data[1]
            os.environ['xIC'] = config_data['dependencies']['microcontroller']
            os.environ['xPLATFORM'] = config_data['dependencies']['platform']
            soft_dev_data = config_data['dependencies']['SoftDevice'].split('_')
            os.environ['xSOFTDEVICE_TYPE'] = soft_dev_data[0]
            os.environ['xSOFTDEVICE_VERSION'] = soft_dev_data[1]
            os.environ['xnRF5_SDK_VERSION'] = config_data['dependencies']['SDK_Version']

    def build(self, target=None):
        if self.build_type == "Debug":
            self.build_debug()
        elif self.build_type == "Release":
            self.build_release()

        if target is None:
            cmd = f"cmake --build {self.build_dir} --target " + self.target
        else:
            cmd = f"cmake --build {self.build_dir} --target " + target
        os.chdir('..')

        if not os.path.isdir(self.build_dir):
            self.build_debug()

        print(f"Building {self.target} in directory {self.build_dir}")
        os.system(cmd)
        print("Done.")

    def build_debug(self):
        cmd = f"cmake -B {self.build_dir} -G \"Unix Makefiles\" -D CMAKE_BUILD_TYPE=Debug ."

        if os.getcwd().split('/')[-1] != PROJECT_BASE_DIR:
            os.chdir('..')
        print("Generating Debug Build files...")
        os.system(cmd)
        print("Done.")

    def build_release(self):
        cmd = f"cmake -B {self.build_dir} -G \"Unix Makefiles\" -D CMAKE_BUILD_TYPE=Release ."

        if os.getcwd().split('/')[-1] != PROJECT_BASE_DIR:
            os.chdir('..')
        print("Generating Release Build files...")
        os.system(cmd)
        print("Done.")

    def build_with_bootloader(self):
        self.build('bl_merge_' + self.target)

    def build_secure_bootloader(self):
        self.build('secure_bootloader_' + self.target)

    def flash(self):
        self.build('flash_' +  self.target)

    def flash_bootloader(self):
        self.build('flash_bl_merge_' + self.target)

    def pkg_dfu(self):
        self.build('pkg_' + self.target)

    def pkg_bl_dfu(self):
        self.build('pkg_bl_sd_' + self.target)

    @staticmethod
    def clean(directory='cmake-build-debug'):
        cmd = "rm -rf " + directory
        print("Cleaning...")
        os.chdir('..')
        os.system(cmd)
        print("Done.")

    @staticmethod
    def logger():
        cmd = "JLinkRTTViewer"
        print("Launching logger viewer...")
        os.system(cmd)

    @staticmethod
    def erase_flash(directory="cmake-build-debug"):
        cmd = "cmake --build " + directory + " --target FLASH_ERASE"
        os.chdir('..')
        os.system(cmd)

    @staticmethod
    def make_venv():
        if os.path.isdir('../venv'):
            print("venv already exists")
        else:
            print("Making venv...")
            os.chdir('..')
            os.system("python -m venv venv")

    @staticmethod
    def start_venv():
        os.system(". ../venv/bin/activate")

    def analyze_map(self):
        map_file = f"{self.build_dir}/src/jd_smartwatch.map"
        cmd = "python analyze_map.py " + map_file
        os.system(cmd)


if __name__ == "__main__":
    bh = BuildHelper()

    bh.read_config()

    if len(sys.argv) >= 2:
        arg = None
        if len(sys.argv) == 3:
            arg = sys.argv[2]

        # print("Arguments: {}".format(sys.argv))
        if arg is not None:
            getattr(bh, sys.argv[1])(arg)
        else:
            getattr(bh, sys.argv[1])()
    else:
        print("Usage: python build_helper.py [option]")
        print("Options:")
        for command in bh.cmds:
            print('\t- {}'.format(command))
