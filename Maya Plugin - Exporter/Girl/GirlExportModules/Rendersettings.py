# ------------------------------------------------------------------------------
# Girl exporter python script plugin for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: Rendersettings.py,v 1.3 2008/07/06 14:46:23 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Render settings export module
#
# ------------------------------------------------------------------------------

import os
from maya import cmds
from ExportModule import ExportModule

class Rendersettings(ExportModule):
    """
    Renderer settings export module
    """
    
    def __init__(self, volumeFile = ''):
        self.volumeFile = volumeFile
    
    def getOutput(self):
        """
        Construct syntax for various parts of Girl's configuration
        """
        
        self.doPixelFilter()
        
        self.doSampler()

        self.doSurfaceIntegrator()
        
        #self.doAccelerator()
        
        # Only set Volume params is volumes present in scene
        if self.volumeFile != '' and os.path.exists(self.volumeFile):
            self.doVolumeIntegrator()

    #end def getOutput
    
    def doVolumeIntegrator(self):
        """
        Volume integrator settings
        """
        
        volume_integrator               = cmds.getAttr( 'Girl_settings.volume_integrator', asString = True ).lower()
        volume_integrator_stepsize      = cmds.getAttr( 'Girl_settings.volume_integrator_stepsize' )
        
        self.addToOutput( 'VolumeIntegrator "%s"' % volume_integrator )
        self.addToOutput( '\t"float stepsize" [%i]' % volume_integrator_stepsize )
        self.addToOutput( '' )
    #end def doVolumeIntegrator
    
    def doAccelerator(self):
        """
        Accelerator settings
        """
        
        accelerator                     = cmds.getAttr( 'Girl_settings.accelerator', asString = True ).lower()
        accelerator_refineimmediately   = self.intToBoolString( cmds.getAttr( 'Girl_settings.accelerator_refineimmediately' ) )
        accelerator_intersectcost       = cmds.getAttr( 'Girl_settings.accelerator_intersectcost' )
        accelerator_traversalcost       = cmds.getAttr( 'Girl_settings.accelerator_traversalcost' )
        accelerator_emptybonus          = cmds.getAttr( 'Girl_settings.accelerator_emptybonus' )
        accelerator_maxprims            = cmds.getAttr( 'Girl_settings.accelerator_maxprims' )
        accelerator_maxdepth            = cmds.getAttr( 'Girl_settings.accelerator_maxdepth' )
        accelerator_filename            = cmds.getAttr( 'Girl_settings.accelerator_KDTreeCacheFileName' )
        accelerator_overwrite           = cmds.getAttr( 'Girl_settings.accelerator_KDTreeCacheOverwrite' )
        
        if (accelerator_overwrite):
                str_overwite = 'true'
        else:
                str_overwite = 'false'

        
        saveFolderorg = cmds.getAttr( 'Girl_settings.scene_path' )
        saveFolderorg = saveFolderorg + os.altsep

        saveFolder = saveFolderorg.replace('\\', '/')
        saveFolder = saveFolder.replace('//', '/')

        saveFolder += 'cache' + os.altsep
        if not os.path.exists(saveFolder):
                os.mkdir( saveFolder )

        accelerator_filename = '../cache/'+accelerator_filename

        self.addToOutput( 'Accelerator "%s"' % accelerator )
        
        if accelerator == 'kdtreecache':
                self.addToOutput( '\t"string filename" ["%s"]' % accelerator_filename )
                self.addToOutput( '\t"bool overwrite" ["%s"]' % str_overwite )
        else:
                if accelerator == 'grid':
                    self.addToOutput( '\t"bool refineimmediately" ["%s"]' % accelerator_refineimmediately )

                if accelerator in ('kdtree','unsafekdtree'):
                    self.addToOutput( '\t"integer intersectcost" [%i]' % accelerator_intersectcost )
                    self.addToOutput( '\t"integer traversalcost" [%i]' % accelerator_traversalcost )
                    self.addToOutput( '\t"float emptybonus" [%f]' % accelerator_emptybonus )
                    self.addToOutput( '\t"integer maxprims" [%i]' % accelerator_maxprims )
                    self.addToOutput( '\t"integer maxdepth" [%i]' % accelerator_maxdepth )


        self.addToOutput( '' )
    #end def doAccelerator
    
    def doSurfaceIntegrator(self):
        """
        Surface integrator settings.
        """
        
        surface_integrator                      = cmds.getAttr( 'Girl_settings.surface_integrator', asString = True ).lower()
        surface_integrator_eyedepth             = cmds.getAttr( 'Girl_settings.surface_integrator_eyedepth' )
        surface_integrator_lightdepth           = cmds.getAttr( 'Girl_settings.surface_integrator_lightdepth' )
        surface_integrator_maxdepth             = cmds.getAttr( 'Girl_settings.surface_integrator_maxdepth' )
        surface_integrator_strategy             = cmds.getAttr( 'Girl_settings.surface_integrator_strategy', asString = True ).lower()
        surface_integrator_rrcontinueprob       = cmds.getAttr( 'Girl_settings.surface_integrator_rrcontinueprob' )
        
        surface_integrator_causticphotons       = cmds.getAttr( 'Girl_settings.surface_integrator_causticphotons' )
        surface_integrator_indirectphotons      = cmds.getAttr( 'Girl_settings.surface_integrator_indirectphotons' )
        surface_integrator_nused                = cmds.getAttr( 'Girl_settings.surface_integrator_nused' )
        surface_integrator_finalgather          = self.intToBoolString( cmds.getAttr( 'Girl_settings.surface_integrator_finalgather' ) )
        surface_integrator_finalgathersamples   = cmds.getAttr( 'Girl_settings.surface_integrator_finalgathersamples' )
        surface_integrator_maxdist              = cmds.getAttr( 'Girl_settings.surface_integrator_maxdist' )
        surface_integrator_gatherangle          = cmds.getAttr( 'Girl_settings.surface_integrator_gatherangle' )
        surface_integrator_diffusedepth         = cmds.getAttr( 'Girl_settings.surface_integrator_diffusedepth' )
        surface_integrator_glossydepth          = cmds.getAttr( 'Girl_settings.surface_integrator_glossydepth' )
        surface_integrator_speculardepth        = cmds.getAttr( 'Girl_settings.surface_integrator_speculardepth' )
        
        self.addToOutput( 'SurfaceIntegrator "%s"' % surface_integrator )
        
        if surface_integrator == 'bidirectional':
            self.addToOutput( '\t"integer eyedepth" [%i]' % surface_integrator_eyedepth )
            self.addToOutput( '\t"integer lightdepth" [%i]' % surface_integrator_lightdepth )
            
        if surface_integrator in ('directlighting', 'path', 'distributedpath'):
            self.addToOutput( '\t"string strategy" ["%s"]' % surface_integrator_strategy )
            
        if surface_integrator == 'particletracing':
            self.addToOutput( '\t"float rrcontinueprob" [%f]' % surface_integrator_rrcontinueprob )
            
        if surface_integrator in ('directlighting','particletracing','path', 'exphotonmap'):
            self.addToOutput( '\t"integer maxdepth" [%i]' % surface_integrator_maxdepth )
            
        if surface_integrator == 'exphotonmap':
            self.addToOutput( '\t"integer causticphotons" [%i]' % surface_integrator_causticphotons )
            self.addToOutput( '\t"integer indirectphotons" [%i]' % surface_integrator_indirectphotons )
            self.addToOutput( '\t"integer nused" [%i]' % surface_integrator_nused )
            self.addToOutput( '\t"bool finalgather" ["%s"]' % surface_integrator_finalgather )
            self.addToOutput( '\t"integer finalgathersamples" [%i]' % surface_integrator_finalgathersamples )
            self.addToOutput( '\t"float maxdist" [%f]' % surface_integrator_maxdist )
            self.addToOutput( '\t"float gatherangle" [%f]' % surface_integrator_gatherangle )
            
        if surface_integrator == 'distributedpath':
            self.addToOutput( '\t"integer diffusedepth" [%i]' % surface_integrator_diffusedepth )
            self.addToOutput( '\t"integer glossydepth" [%i]' % surface_integrator_glossydepth )
            self.addToOutput( '\t"integer speculardepth" [%i]' % surface_integrator_speculardepth )
            
        self.addToOutput( '' )
    #end def doSurfaceIntegrator
    
    def doSampler(self):
        """
        Sampler settings
        """
        
        pixel_sampler                   = cmds.getAttr( 'Girl_settings.pixel_sampler', asString = True ).lower().replace(' ','')
        pixel_sampler_pixelsampler      = cmds.getAttr( 'Girl_settings.pixel_sampler_pixelsampler', asString = True ).lower()
        pixel_sampler_xsamples          = cmds.getAttr( 'Girl_settings.pixel_sampler_xsamples' )
        pixel_sampler_ysamples          = cmds.getAttr( 'Girl_settings.pixel_sampler_ysamples' )
        pixel_sampler_pixelsamples      = cmds.getAttr( 'Girl_settings.pixel_sampler_pixelsamples' )
        pixel_sampler_initsamples       = cmds.getAttr( 'Girl_settings.pixel_sampler_initsamples' )
        #pixel_sampler_mutationrange     = cmds.getAttr( 'Girl_settings.pixel_sampler_mutationrange' )
        pixel_sampler_maxconsecrejects  = cmds.getAttr( 'Girl_settings.pixel_sampler_maxconsecrejects' )
        pixel_sampler_largemutationprob = cmds.getAttr( 'Girl_settings.pixel_sampler_largemutationprob' )
        pixel_sampler_chainlength       = cmds.getAttr( 'Girl_settings.pixel_sampler_chainlength' )
        pixel_sampler_usevariance       = self.intToBoolString( cmds.getAttr( 'Girl_settings.pixel_sampler_usevariance' ) )
        
        self.addToOutput( 'Sampler "%s"' % pixel_sampler )
        
        if pixel_sampler == "random":
            self.addToOutput( '\t"integer xsamples" [%i]' % pixel_sampler_xsamples )
            self.addToOutput( '\t"integer ysamples" [%i]' % pixel_sampler_ysamples )
        
        if pixel_sampler in ('random','lowdiscrepancy','halton'):
            self.addToOutput( '\t"string pixelsampler" ["%s"]' % pixel_sampler_pixelsampler )

        if pixel_sampler in ('lowdiscrepancy','halton'):
            self.addToOutput( '\t"integer pixelsamples" [%i]' % pixel_sampler_pixelsamples )
        
        if pixel_sampler == "metropolis":
            self.addToOutput( '\t"integer maxconsecrejects" [%i]' % pixel_sampler_maxconsecrejects )
            self.addToOutput( '\t"float largemutationprob" [%f]' % pixel_sampler_largemutationprob )
            self.addToOutput( '\t"bool usevariance" ["%s"]' % pixel_sampler_usevariance )
            
        if pixel_sampler == 'erpt':
            self.addToOutput( '\t"integer chainlength" [%i]' % pixel_sampler_chainlength )
        
        if pixel_sampler in ('metropolis','erpt'):
            self.addToOutput( '\t"integer initsamples" [%i]' % pixel_sampler_initsamples )
            #self.addToOutput( '\t"float mutationrange" [%f]' % pixel_sampler_mutationrange )
            
        self.addToOutput( '' )
    #end def doSampler
    
    def doPixelFilter(self):
        """
        Filter settings
        """
        
        pixel_filter        = cmds.getAttr( 'Girl_settings.pixel_filter', asString = True ).lower()
        pixel_filter_xwidth = cmds.getAttr( 'Girl_settings.pixel_filter_xwidth' )
        pixel_filter_ywidth = cmds.getAttr( 'Girl_settings.pixel_filter_ywidth' )
        pixel_filter_b      = cmds.getAttr( 'Girl_settings.pixel_filter_b' )
        pixel_filter_c      = cmds.getAttr( 'Girl_settings.pixel_filter_c' )
        pixel_filter_alpha  = cmds.getAttr( 'Girl_settings.pixel_filter_alpha' )
        pixel_filter_tau    = cmds.getAttr( 'Girl_settings.pixel_filter_tau' )
        
        self.addToOutput( 'PixelFilter "%s"' % pixel_filter )
        self.addToOutput( '\t"float xwidth" [%f]' % pixel_filter_xwidth )
        self.addToOutput( '\t"float ywidth" [%f]' % pixel_filter_ywidth )
        
        if pixel_filter == "mitchell":
            self.addToOutput( '\t"float B" [%f]' % pixel_filter_b )
            self.addToOutput( '\t"float C" [%f]' % pixel_filter_c )
        
        if pixel_filter == "gaussian":
            self.addToOutput( '\t"float alpha" [%f]' % pixel_filter_alpha )
            
        if pixel_filter == "sinc":
            self.addToOutput( '\t"float tau" [%f]' % pixel_filter_tau )
            
        self.addToOutput( '' )
    #end def doPixelFilter