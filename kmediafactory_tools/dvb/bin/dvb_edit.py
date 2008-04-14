#!/usr/bin/python

from kmediafactory import uiInterface, projectInterface
import os
from popen2 import Popen4
import time 

APP = 'KMediaFactory'

class DVBPlugin:
    def __init__(self):
        uiInterface().addMediaAction('video-television', # Icon
                                     'Add DV&B',         # Text
                                     'Ctrl+B',           # Shortcut
                                     'dvb_edit',         # Name
                                     self,               # Object
                                     'edit'              # Method
        )
        
    def run(self, cmd):
        uiInterface().debug(cmd)
        pipe = Popen4(cmd)
        result = pipe.wait()
        output = pipe.fromchild.read()
        return(result, output.strip())
    
    def projectx(self):
        result = self.run('which projectx')
        if result[0] == 0:
            return result[1]
        result = self.run('locate ProjectX.jar')
        if result[0] == 0:
            return 'java -jar ' + result[1]
        if os.path.exists('/usr/share/projectx/ProjectX.jar'):
            return 'java -jar /usr/share/projectx/ProjectX.jar'
        return 'java -jar ProjectX.jar'
    
    def edit(self):
        ts_files = uiInterface().getOpenFileNames('KMF_DVB', 
                                                  '*.ts *.ps *.mpg *.mpeg|Mpeg files',
                                                  'Add DVB file')

        if len(ts_files) != 0:
            files = ''
            for f in ts_files:
                files += '"' + f + '" '
            projectDir = projectInterface().projectDir('media')
            result = self.run(self.projectx() + ' -gui ' + files + ' -out "' + projectDir + '"')
            if result[0] == 0:
                base = os.path.splitext(os.path.basename(ts_files[0]))[0]
                input_audio_ac3 = projectDir + base + '.ac3'
                input_audio_mp2 = projectDir + base + '.mp2'
                input_video = projectDir + base + '.m2v'
                output = projectDir + base + '.mpg'
        
                if os.path.exists(input_audio_ac3):
                    input_audio = input_audio_ac3
                else:
                    input_audio = input_audio_mp2
                try:
                    size = os.path.getsize(input_audio) + os.path.getsize(input_video)
                except:
                    uiInterface().messageBox(APP, 'Making %s failed.' % output,
                                             kmediafactory.Error)
                    return
                dlg = uiInterface().progressDialog(APP, 'Making %s...' % output,
                                                   (size / 1024))
        
                cmd = 'mplex -f 8 -o "' + output + '" "' + input_video + '" "' + \
                      input_audio + '"'
                pipe = Popen4(cmd)
                while True:
                    time.sleep(0.25)
                    try:
                        size = os.path.getsize(output) / 1024
                    except:
                        size = 0
                    dlg.setValue(size)
                    if dlg.wasCancelled():
                        os.kill(pipe.pid)
                        break
                    n = pipe.poll()
                    if n != -1:
                        break
                dlg.close()
                if n == 0:
                    xml = '<media plugin="KMFImportVideo" object="video">' + \
                          '<video><file path="' + output + '"></video></media>'
                    uiInterface().addMediaObjectFromXML(xml)
                else:
                    uiInterface().messageBox(APP, 'Making %s failed.' % output,
                                             kmediafactory.Error)
            else:
                uiInterface().messageBox(APP, 'Running ProjectX failed.',
                                         kmediafactory.Error)

    def initPlugin(self, projectType):
        uiInterface().setActionEnabled('dvb_edit', projectType[:3] == 'DVD')

    def supportedProjectTypes(self):
        return ['DVD-PAL', 'DVD-NTSC']

kmediafactory.registerPlugin(DVBPlugin())
