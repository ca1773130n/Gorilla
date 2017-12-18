# ------------------------------------------------------------------------------
# Girl exporter python script plugin for Maya
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: Girl_batch.py,v 1.20 2008/07/08 15:01:43 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl_batch: a command to render a set of Girl renders (sequential, or network)
#
# ------------------------------------------------------------------------------

import os
os.altsep = '/'

import sys, math
import maya.OpenMaya as OpenMaya

from maya import cmds
from maya import OpenMaya
from maya import OpenMayaMPx
from maya import OpenMayaUI
from maya import OpenMayaRender


from Girl_export	import Girl_export
from Girl_export	import consoleProgress 
from Girl_view		import Girl_view
from Girl_launch	import Girl_launch

from struct import *

class Girl_render(OpenMayaMPx.MPxCommand):
    """
    Class to create a render batch script
    """
    tgaSaveName = ''

    @staticmethod
    def commandName():
        return "Girl_render"
    
    @staticmethod
    def commandCreator():
        return OpenMayaMPx.asMPxPtr( Girl_render() )
    
    def doIt(self, args = OpenMaya.MArgList() ):
        """
        Class entry point.
        1. Detect if Girl_settings exists
        2. Detect if exporting an animation
        3. Determine start and end frame to export, and pass control to startBatch()
        """
        
        if not cmds.objExists('Girl_settings'):
            OpenMaya.MGlobal.displayWarning('No Girl settings found for this scene, opening GUI...')
            from Girl_gui import Girl_gui
            lg = Girl_gui()
            lg.doIt(OpenMaya.MArgList())
            return
        
        # here we go
        doAnimation  = cmds.getAttr( 'Girl_settings.render_animation' )
        doInSequence = cmds.getAttr( 'Girl_settings.render_animation_sequence' )
        
        self.startFrame = round( cmds.currentTime( query = True ) )
        
        if doAnimation:
            self.startFrame  = round( cmds.getAttr( 'defaultRenderGlobals.startFrame'  ) ) 
            self.endFrame    = round( cmds.getAttr( 'defaultRenderGlobals.endFrame'    ) )
            self.stepFrame   = round( cmds.getAttr( 'defaultRenderGlobals.byFrameStep' ) )
        else:
            self.endFrame = self.startFrame
        
        self.initProgressWindow()
        
        if doInSequence:
            self.startSequence()
        else:
            self.startBatch()
    
    def initProgressWindow(self):
        self.MayaGUIMode = OpenMaya.MGlobal.mayaState() == OpenMaya.MGlobal.kInteractive
        
        if self.MayaGUIMode:
            self.mProgress = OpenMayaUI.MProgressWindow()
        else:
            self.mProgress = consoleProgress()
            
    def showProgressWindow(self):
        if self.MayaGUIMode:
            self.mProgress.reserve()
            self.mProgress.setInterruptable(True)
            self.mProgress.setProgressRange(0, int(self.endFrame-self.startFrame)+1)
            self.mProgress.setProgress(0)
            self.mProgress.startProgress()
        else:
            self.mProgress.tProgress = int(self.endFrame-self.startFrame)+1
    
    def startSequence(self):
        """
        Start the sequential export process.
        1. for each frame to export, export it to a temp folder
        2. render it
        3. loop
        """
        
        self.showProgressWindow()
        
        if self.startFrame == self.endFrame:
            self.runProcess( self.exportFile(self.startFrame) )
            self.mProgress.advanceProgress(1)
        else:
            # frame range export
            ct = cmds.currentTime( query = True )
            for f in range(int(self.startFrame), int(self.endFrame)+1, int(self.stepFrame)): 
                self.mProgress.setTitle( 'Frames %i - %i: %i' % (int(self.startFrame), int(self.endFrame), f) )
                cmds.currentTime( f )
                self.runProcess( self.exportFile(f, tempExportPath = True) )
                self.mProgress.advanceProgress(1)
                if self.mProgress.isCancelled(): break
            
            cmds.currentTime( ct )
        
        OpenMaya.MGlobal.displayInfo( 'Girl Export Successful' )
        self.mProgress.endProgress()
    
    def startBatch(self):
		"""
		Start the batch export process.
		1. For each frame to export, export it
		2. Append exported scene filename to fileList
		3. pass fileList to makeBatchFile()
		"""

		fileList = []
		girlSceneFile = ''

		self.showProgressWindow()

		if self.startFrame == self.endFrame:
			# single frame export
			girlSceneFile = self.exportFile(self.startFrame)
			fileList.append( girlSceneFile )
			self.mProgress.advanceProgress(1)
		else:
			# frame range export
			ct = cmds.currentTime( query = True )

			for f in range(int(self.startFrame), int(self.endFrame)+1, int(self.stepFrame)): 
				self.mProgress.setTitle( 'Frames %i - %i: %i' % (int(self.startFrame), int(self.endFrame), f) )
				cmds.currentTime( f )
				girlSceneFile = self.exportFile(f)
				fileList.append( girlSceneFile )
				self.mProgress.advanceProgress(1)

				if self.mProgress.isCancelled(): 
					break

			cmds.currentTime( ct )

		if self.startFrame == self.endFrame:
			run_id = self.runGirlProcess(fileList)
			OpenMaya.MGlobal.displayInfo( 'Girl Export Successful' )
			gv_thread = Girl_view( self.tgaSaveName, run_id, 1 )
			OpenMaya.MGlobal.displayInfo( 'Start Girl View Thread' )
			gv_thread.start()
			self.mProgress.endProgress()
		else:
			self.makeBatchFile(fileList)
			OpenMaya.MGlobal.displayInfo( 'Girl Export Successful' )
			self.mProgress.endProgress()

		"""    
		gl = Girl_launch()
		glArgs = OpenMaya.MArgList()
		glArgs.addArg( girlSceneFile )
		glArgs.addArg( cmds.getAttr( 'Girl_settings.Girl_path' ) )
		glArgs.addArg( cmds.getAttr( 'Girl_settings.render_interface' ) )
		glArgs.addArg( cmds.getAttr( 'Girl_settings.render_threads' ) )
		glArgs.addArg( cmds.getAttr( 'Girl_settings.render_priority' ) )
		gl.doIt( glArgs )
        """
		# launch view thread


    def exportFile(self, frameNumber = 1, tempExportPath = False):
		"""
		Export a single frame, and return the name of the created scene file
		"""

		render_cam = ''
		cam_idx = cmds.getAttr( 'Girl_settings.camera_name' )
		seekidx = 1

		if cam_idx == 0:
			for cam in cmds.listCameras():
				renderable = cmds.getAttr( '%s.renderable' % cam )
				if renderable == 1:
					render_cam = cam
					break
		else:
			for cam in cmds.listCameras():
				if ( cam != 'front' and cam != 'persp' and cam != 'side' and cam != 'top' ):
					if cam_idx == seekidx:
						render_cam = cam
						break
					seekidx = seekidx + 1
		#OpenMaya.MGlobal.displayInfo(cam)

		if render_cam == '':
			OpenMaya.MGlobal.displayError('No renderable camera in scene')


		saveFolderorg = cmds.getAttr( 'Girl_settings.scene_path' )
                saveFolderorg = saveFolderorg + os.altsep

                saveFolder = saveFolderorg.replace('\\', '/')
                saveFolder = saveFolderorg.replace('//', '/')

		if not os.path.exists(saveFolder):
			os.mkdir( saveFolder )

		sceneFileBaseName = cmds.getAttr( 'Girl_settings.scene_filename' ) + ('.%06i' % frameNumber)

		renderFolder = saveFolder + "renders" + os.altsep
		if not os.path.exists(renderFolder):
			os.mkdir(renderFolder)

		imageSaveName = renderFolder + sceneFileBaseName

		if tempExportPath:
			saveFolder += ('tmp') + os.altsep
			if not os.path.exists(saveFolder):
				os.mkdir( saveFolder )
			else:
				for file in os.listdir(saveFolder):
					os.remove(saveFolder+file)
					os.rmdir( saveFolder )
					os.mkdir( saveFolder )
		else:
			saveFolder += ('%06i' % frameNumber) + os.altsep
			if not os.path.exists(saveFolder):
				os.mkdir( saveFolder )

		sceneFileName = saveFolder + sceneFileBaseName + '.girls'

		renderWidth = cmds.getAttr( 'defaultResolution.width' )
		renderHeight = cmds.getAttr( 'defaultResolution.height' )

		self.tgaSaveName = imageSaveName +'.tga'

		return sceneFileName
    
    def getNetworkServers(self):
		networking = cmds.getAttr( 'Girl_settings.render_network' )
		serversList = cmds.getAttr( 'Girl_settings.render_network_servers')

		if networking and serversList != None and len(serversList.strip()) > 0:
			serversList = serversList.strip().split(';')
			for item in serversList:
				if len(item.strip()) == 0: serversList.remove(item)
				
			if len(serversList) > 0:
				servers = str()
				for server in serversList:
					servers += '-u %s ' % server

				networkinterval = cmds.getAttr( 'Girl_settings.render_network_interval' )                
				servers += '-i %i ' % networkinterval
			
				#return servers
				return ''
			else:
				return ''
		else:
			return ''
    

    def runGirlProcess(self, fileList):
        """
        Make batch render file, detecting appropriate type for the host OS.
        1. Add all given file names in fileList to the batch script
        2. Launch the script if told to. 
        """
        
        GirlPath = cmds.getAttr( 'Girl_settings.Girl_path' ) + os.altsep
        guiMode = cmds.getAttr( 'Girl_settings.render_interface', asString = True ) == 'GUI'
        threads = cmds.getAttr( 'Girl_settings.render_threads' )
        priority = cmds.getAttr( 'Girl_settings.render_priority', asString = True )
        
        GirlPath = GirlPath.replace('/', '\\')
        GirlPath = GirlPath.replace('\\\\', '\\')

        servers = self.getNetworkServers()
                
        # scale 0...5 to -9...6 - keeping normal at 0
        niceValue = (cmds.getAttr( 'Girl_settings.render_priority' ) - 3) * 3
        
        if guiMode:
            GirlPath += 'girlviewer'
        else:
            GirlPath += 'GirlConsole'

        if os.name == 'nt':
            GirlPath += '.exe'
        else:
            OpenMaya.MGlobal.displayError( 'Could not write render batch file' )
            #unix bash script
            """
            batchFileName += '.sh'
            try:
                fh = open( batchFileName, 'wb')
                fh.write( '#!/bin/bash' + os.linesep)
                fh.write( '# Girl batch render file generated by GirlMaya ' + cmds.date() + os.linesep )
                
                
                for file in fileList:
                    fh.write( ('nice -n %i %s -t %i %s"%s"' % (niceValue, GirlPath, threads, servers, file)) + os.linesep )
                fh.close()
            except:
                OpenMaya.MGlobal.displayError( 'Could not write render batch file' )
                raise
            """
        
        try:
            if os.name == 'nt':
                # windows
                for file in fileList:
                        process_id = os.spawnv(os.P_NOWAIT, GirlPath, ['"%s"' % GirlPath, '"%s"' % file ] )
                        #process_id = os.spawnv(os.P_NOWAIT, batchFileName, ['"%s"' % batchFileName] )
                        #os.system('start cmd /c "%s"' % batchFileName)
            else:
                # assuming linux, this probably won't work on OSX
                os.system('(xterm -T "Girl Render" -e %s)&' % batchFileName)
        except:
            OpenMaya.MGlobal.displayError( "Failed to launch process\n" )
            raise


        return process_id
        #OpenMaya.MGlobal.displayInfo( "Batch Success!\n" )

    def makeBatchFile(self, fileList):
        """
        Make batch render file, detecting appropriate type for the host OS.
        1. Add all given file names in fileList to the batch script
        2. Launch the script if told to. 
        """
        
        saveFolderorg = cmds.getAttr( 'Girl_settings.scene_path' )
        saveFolderorg = saveFolderorg + os.altsep

        saveFolder = saveFolderorg.replace('\\', '/')
        saveFolder = saveFolder.replace('//', '/')

        sceneFileBaseName = cmds.getAttr( 'Girl_settings.scene_filename' )
        batchFileName = saveFolder + sceneFileBaseName + '_render'
        GirlPath = cmds.getAttr( 'Girl_settings.Girl_path' ) + os.altsep
        guiMode = cmds.getAttr( 'Girl_settings.render_interface', asString = True ) == 'GUI'
        threads = cmds.getAttr( 'Girl_settings.render_threads' )
        priority = cmds.getAttr( 'Girl_settings.render_priority', asString = True )
        
        GirlPath = GirlPath.replace('/', '\\')
        GirlPath = GirlPath.replace('\\\\', '\\')

        servers = self.getNetworkServers()
                
        # scale 0...5 to -9...6 - keeping normal at 0
        niceValue = (cmds.getAttr( 'Girl_settings.render_priority' ) - 3) * 3
        
        if guiMode:
            GirlPath += 'girlviewer'
        else:
            GirlPath += 'GirlConsole'

        if os.name == 'nt':
            GirlPath += '.exe'
            # windows batch file
            batchFileName += '.bat'
            try:
                fh = open( batchFileName, 'wb')
                fh.write( ':: Girl batch render file generated by GirlMaya ' + cmds.date() + os.linesep )
                fh.write( '@Echo off' + os.linesep )
                
                cmdPrefix = str()
                if guiMode:
                    cmdPrefix = 'start /WAIT /%s' % priority
                else:
                    cmdPrefix = 'start /WAIT /MIN /%s' % priority
                
                for file in fileList:
                    ccmd = '%s %s %s"%s"' % ( cmdPrefix, GirlPath, servers, file )
                    fh.write( ('echo Rendering file %s' % file) + os.linesep )
                    fh.write( ccmd + os.linesep )
                    
                fh.close()
            except:
                OpenMaya.MGlobal.displayError( 'Could not write render batch file' )
                raise
            
        else:
            #unix bash script
            batchFileName += '.sh'
            try:
                fh = open( batchFileName, 'wb')
                fh.write( '#!/bin/bash' + os.linesep)
                fh.write( '# Girl batch render file generated by GirlMaya ' + cmds.date() + os.linesep )
                
                
                for file in fileList:
                    fh.write( ('nice -n %i %s -t %i %s"%s"' % (niceValue, GirlPath, threads, servers, file)) + os.linesep )
                fh.close()
            except:
                OpenMaya.MGlobal.displayError( 'Could not write render batch file' )
                raise
        

        if cmds.getAttr( 'Girl_settings.render_launch' ) == 1:
            try:
                if os.name == 'nt':
                    # windows
                    #os.spawnv(os.P_NOWAIT, batchFileName, [batchFileName] )
                    os.system('start cmd /c "%s"' % batchFileName)
                else:
                    # assuming linux, this probably won't work on OSX
                    os.system('(xterm -T "Girl Render" -e %s)&' % batchFileName)
            except:
                OpenMaya.MGlobal.displayError( "Failed to launch process\n" )
                raise


    def runProcess(self, sceneFile):
        self.mProgress.setProgressStatus( 'Rendering' )

        GirlPath = cmds.getAttr( 'Girl_settings.Girl_path' ) + os.altsep
        guiMode = cmds.getAttr( 'Girl_settings.render_interface', asString = True ) == 'GUI'
        threads = cmds.getAttr( 'Girl_settings.render_threads' )
        priority = cmds.getAttr( 'Girl_settings.render_priority', asString = True )
        # scale 0...5 to -9...6 - keeping normal at 0
        niceValue = (cmds.getAttr( 'Girl_settings.render_priority' ) - 3) * 3
        
        GirlPath = GirlPath.replace('/', '\\')
        GirlPath = GirlPath.replace('\\\\', '\\')

        servers = self.getNetworkServers()
        
        if guiMode:
            GirlPath += 'girlviewer'
        else:
            GirlPath += 'GirlConsole'
            
        if os.name == 'nt':
            GirlPath += '.exe'
            
        if os.name == 'nt':
            # windows batch file
            try:
                cmdPrefix = str()
                if guiMode:
                    cmdPrefix = 'start /WAIT /%s' % priority
                else:
                    cmdPrefix = 'start /WAIT /MIN /%s' % priority
                
                ccmd = '%s %s -t %i %s"%s"' % ( cmdPrefix, GirlPath, threads, servers, sceneFile )
                os.system( ccmd )
                
            except:
                OpenMaya.MGlobal.displayError( 'Could not start Girl' )
                raise
        else:
            try:
                cmdPrefix = 'nice -n %i %s %s"%s"' % (niceValue, GirlPath, servers, sceneFile)
                if guiMode:
                    ccmd = '(xterm -T "Girl Render" -e %s)' & cmdPrefix
                else:
                    ccmd = '(%s)' % cmdPrefix
                    
                os.system( ccmd )
            except:
                OpenMaya.MGlobal.displayError( 'Could not start Girl' )
                raise
    
