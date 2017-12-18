# ------------------------------------------------------------------------------
# Girl exporter python script plugin for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: FileCollector.py,v 1.3 2008/06/13 21:23:00 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Texture file collector method
#
# ------------------------------------------------------------------------------

import os, shutil
os.altsep = '/'

from maya import cmds
from maya import OpenMaya

class FileCollector:
    """
    Texture file collector for export process.
    TODO: bump maps will be filed under textures because of the generic
    routines. Find a way to make it wok again.
    """
    
    @staticmethod
    def collectTexture( texFile ):
        collectEnabled = cmds.getAttr( 'Girl_settings.scene_collect_texture' )
        if collectEnabled:
            return FileCollector.doCollect( fileName = texFile, targetPath = 'textures' )
        else:
            return texFile
        
    @staticmethod
    def collectBumpmap( texFile ):
        collectEnabled = cmds.getAttr( 'Girl_settings.scene_collect_bump' )
        if collectEnabled:
            return FileCollector.doCollect( fileName = texFile, targetPath = 'bumpmaps' )
        else:
            return texFile
        
    @staticmethod
    def collectHDRI( texFile ):
        collectEnabled = cmds.getAttr( 'Girl_settings.scene_collect_hdri' )
        if collectEnabled:
            return FileCollector.doCollect( fileName = texFile, targetPath = 'environments' )
        else:
            return texFile
        
    @staticmethod
    def doCollect( fileName, targetPath ):
        
        if not os.path.exists( fileName ):
            OpenMaya.MGlobal.displayWarning( "File %s doesn't exist" % fileName )
        else:
            scenePath = cmds.getAttr( 'Girl_settings.scene_path' )
            newFilePath = scenePath + os.altsep + targetPath + os.altsep
            if not os.path.exists( newFilePath ):
                os.mkdir( newFilePath )
            fileBaseName = os.path.basename( fileName )
            newFileName = newFilePath + fileBaseName
            if not os.path.exists( newFileName ):
                shutil.copyfile( fileName, newFileName )
            fileName = '..' + os.altsep + targetPath + os.altsep + fileBaseName
        
        return fileName