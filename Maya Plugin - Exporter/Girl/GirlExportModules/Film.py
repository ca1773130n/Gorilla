# ------------------------------------------------------------------------------
# Girl exporter python script plugin for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: Film.py,v 1.3 2008/06/10 20:28:29 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Film settings export module
#
# ------------------------------------------------------------------------------
import os
os.altsep = '/'

from ExportModule import ExportModule

from maya import cmds

class Film(ExportModule):
    """
    Girl "Film" ExportModule. Responsible for getting the scene's Film settings.
    """
    
    def __init__(self, nWidth, nHeight, nImageName):
        """
        Constructor. Set's up this object with required data.
        """
        
        self.width = nWidth
        self.height = nHeight
        self.imageName = nImageName
        
    def getOutput(self):
        """
        Append this module's output to the scene file buffer.
        """
                
        # Get vars
        # TODO crop window could come from defaultRenderGlobals
        cx1             = cmds.getAttr( 'Girl_settings.film_cropwindowXmin' )
        cx2             = cmds.getAttr( 'Girl_settings.film_cropwindowXmax' )
        cy1             = cmds.getAttr( 'Girl_settings.film_cropwindowYmin' )
        cy2             = cmds.getAttr( 'Girl_settings.film_cropwindowYmax' )
        gamma           = cmds.getAttr( 'Girl_settings.film_gamma' )
        prealpha        = self.intToBoolString( cmds.getAttr( 'Girl_settings.film_premultiplyalpha' ) )
        
        tm_exr          = self.intToBoolString( cmds.getAttr( 'Girl_settings.film_write_tonemapped_exr' ) )
        tm_igi          = self.intToBoolString( cmds.getAttr( 'Girl_settings.film_write_tonemapped_igi' ) )
        tm_tga          = self.intToBoolString( cmds.getAttr( 'Girl_settings.film_write_tonemapped_tga' ) )
        utm_exr         = self.intToBoolString( cmds.getAttr( 'Girl_settings.film_write_untonemapped_exr' ) )
        utm_igi         = self.intToBoolString( cmds.getAttr( 'Girl_settings.film_write_untonemapped_igi' ) )
        resume          = self.intToBoolString( cmds.getAttr( 'Girl_settings.film_write_resume_film' ) )
        restart        = self.intToBoolString( cmds.getAttr( 'Girl_settings.film_restart_resume_film' ) )
        
        r_prescale      = cmds.getAttr( 'Girl_settings.film_reinhard_prescale' )
        r_postscale     = cmds.getAttr( 'Girl_settings.film_reinhard_postscale' )
        r_burn          = cmds.getAttr( 'Girl_settings.film_reinhard_burn' )
        
        writeinterval   = cmds.getAttr( 'Girl_settings.film_writeinterval' )
        displayinterval = cmds.getAttr( 'Girl_settings.film_displayinterval' )
        reject_warmup   = cmds.getAttr( 'Girl_settings.film_reject_warmup' )
        
        haltspp         = cmds.getAttr( 'Girl_settings.film_haltspp' )
        
        
        # send to Film output
        self.addToOutput( 'Film "fleximage"' )
        self.addToOutput( '\t"integer xresolution" [%i]' % self.width )
        self.addToOutput( '\t"integer yresolution" [%i]' % self.height )

        saveFolderorg = cmds.getAttr( 'Girl_settings.scene_path' )
        saveFolderorg = saveFolderorg + os.altsep
        saveFolder = saveFolderorg.replace('\\', '/')    
        saveFolder = saveFolder.replace('//', '/')

        saveImageName = self.imageName.replace(saveFolder, '../')
        
        self.addToOutput( '\t"string filename" ["%s"]' % saveImageName )
        
        self.addToOutput( '\t"float cropwindow" [%f %f %f %f]' % ( cx1, cx2, cy1, cy2 ) )
        self.addToOutput( '\t"float gamma" [%f]' % gamma )
        self.addToOutput( '\t"bool premultiplyalpha" ["%s"]' % prealpha )
        
        self.addToOutput( '\t"bool write_tonemapped_exr" ["%s"]' % tm_exr )
        self.addToOutput( '\t"bool write_tonemapped_igi" ["%s"]' % tm_igi )
        self.addToOutput( '\t"bool write_tonemapped_tga" ["%s"]' % tm_tga )
        self.addToOutput( '\t"bool write_untonemapped_exr" ["%s"]' % utm_exr )
        self.addToOutput( '\t"bool write_untonemapped_igi" ["%s"]' % utm_igi )
        
        
        self.addToOutput( '\t"bool write_resume_flm" ["%s"]' % resume )
        self.addToOutput( '\t"bool restart_resume_flm" ["%s"]' % restart )
        
        self.addToOutput( '\t"float reinhard_prescale" [%f]'  % r_prescale )
        self.addToOutput( '\t"float reinhard_postscale" [%f]' % r_postscale )
        self.addToOutput( '\t"float reinhard_burn" [%f]' % r_burn )
        
        self.addToOutput( '\t"integer writeinterval" [%i]' % writeinterval )
        self.addToOutput( '\t"integer displayinterval" [%i]' % displayinterval )
        self.addToOutput( '\t"integer reject_warmup" [%i]' % reject_warmup )
        
        self.addToOutput( '\t"integer haltspp" [%i]' % haltspp )
        
        self.addToOutput( '' )
    #end def getOutput