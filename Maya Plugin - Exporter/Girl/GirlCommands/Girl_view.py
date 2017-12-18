# ------------------------------------------------------------------------------
# Girl exporter python script plugin for Maya
#
# Based on a translation of the c++ Girlmaya exporter, in turn based on
# maya-pbrt by Mark Colbert
#
# Python translation by Doug Hammond 02/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: Girlview.py 69 2008-05-20 16:44:36Z doug $
#
# ------------------------------------------------------------------------------
#
# Girlview: a command to view render output.
# 	currently this is achieved in 1 of 2 ways:
#	1. Launch the external fcheck program
#	2. Parse and load a TGA file into the render window
#
#	I know which method i prefer... 
#
# ------------------------------------------------------------------------------

import math, os, re, time, sys

from threading	import Thread
from maya		import OpenMaya
from maya		import OpenMayaMPx
from maya		import OpenMayaRender

class ProcessDetectThread ( Thread ):

	def __init__ (self, pGirlView, pID):
		Thread.__init__(self)
		self.m_pGirlView = pGirlView
		self.m_pID = pID
	
	def run(self):
		ret = os.waitpid(self.m_pID,0)
		self.m_pGirlView.ProcessStopNotify()


class Girl_view( Thread ):

	def __init__ (self, szRenderFileName, pProcessID, iInterval):
		Thread.__init__(self)
		self.m_szRenderFileName = szRenderFileName
		self.m_pProcessID = pProcessID
		self.m_bDebug = False
		self.m_bStopThread = 0
		self.m_iInterval = iInterval
		self.m_bLock = 0
		self.m_iLastUpdateTime = 0
		self.m_NotifyThread = ProcessDetectThread(self, self.m_pProcessID)

	def run( self ):
		#ret = os.waitpid(self.m_pProcessID,0)
		self.m_NotifyThread.start()

		time.sleep(self.m_iInterval)

		while not self.m_bStopThread :
			if (not OpenMayaRender.MRenderView.doesRenderEditorExist()):
				OpenMaya.MGlobal.displayError("Cannot Girl_view in batch render mode.")
				return

			if ( os.path.exists(self.m_szRenderFileName) ):
				if ( not self.m_bLock ) :
					iFileUpdateTime = os.path.getmtime(self.m_szRenderFileName)
					if ( self.m_iLastUpdateTime != iFileUpdateTime ):
						try:
							OpenMaya.MGlobal.displayInfo( "Start Rendering" )
							self.loadTgaIntoRenderView(self.m_szRenderFileName)
							OpenMaya.MGlobal.displayInfo( "End Rendering" )
						except:
							self.m_bStopThread = 1
							raise
						self.m_iLastUpdateTime = iFileUpdateTime
					#else:
					#	OpenMaya.MGlobal.displayInfo("Waiting for tga file update")

			else:
				OpenMaya.MGlobal.displayInfo("image file check \""+self.m_szRenderFileName+"\"")

			if ( self.m_bStopThread ) : break
			time.sleep(self.m_iInterval)

	def ProcessStopNotify(self):
		self.m_bStopThread = 1
		#self.m_NotifyThread.join()
		#if ( self.isAlive() ): self.join()

	def loadTgaIntoRenderView(self, file):
		# Start file read

		if ( self.m_bLock ) : return

		self.m_bLock = 1

		try:
			fptr = open(file, "rb")
		except:
			OpenMaya.MGlobal.displayError( "Failed to open file\n" )
			raise
		
		try:
			headerbytes = fptr.read(18)
		except:
			OpenMaya.MGlobal.displayError( "Unexpected EOF\n" )
			raise
		
		tgaIdLength = ord(headerbytes[0])
		self.dprint ("IdLength: " + str(tgaIdLength))
		
		tgaColorMapType = ord(headerbytes[1])
		self.dprint ("ColorMapType: " + str(tgaColorMapType))
		
		tgaDataTypeCode = ord(headerbytes[2])
		self.dprint ("DataTypeCode: " + str(tgaDataTypeCode))
		
		tgaColorMapLength = (( ord(headerbytes[6]) << 8) | 0x00FF) & ( ord(headerbytes[5]) | 0xFF00)
		self.dprint ("ColorMapLength: " + str(tgaColorMapLength))
		
		tgaColorMapDepth = (( ord(headerbytes[8]) << 8) | 0x00FF) & ( ord(headerbytes[7]) | 0xFF00)
		self.dprint ("ColorMapDepth: " + str(tgaColorMapDepth))
		
		tgaWidth = (( ord(headerbytes[13]) << 8) | 0x00FF) & ( ord(headerbytes[12]) | 0xFF00)
		self.dprint ("Width: " + str(tgaWidth))
		
		tgaHeight = (( ord(headerbytes[15]) << 8) | 0x00FF) & ( ord(headerbytes[14]) | 0xFF00)
		self.dprint ("Height: " + str(tgaHeight))
		
		tgaBitdepth = ord(headerbytes[16])
		self.dprint ("Bitdepth: " + str(tgaBitdepth))
		
		if tgaDataTypeCode not in (2, 10):
			fptr.close()
			OpenMaya.MGlobal.displayError( "Can only handle image type 2 and 10\n" )
			return
			
		if tgaBitdepth not in (24, 32):
			fptr.close()
			OpenMaya.MGlobal.displayError( "Can only handle pixel depths of 24 and 32\n" )
			return
			
		if tgaColorMapType not in (0, 1):
			fptr.close()
			OpenMaya.MGlobal.displayError( "Can only handle color map types of 0 and 1\n" )
			return
		
		skipover = tgaIdLength + tgaColorMapDepth*tgaColorMapLength
		
		fptr.seek(skipover, 1) # 1 == SEEK_CUR
		
		byteCount = tgaBitdepth / 8
		n = 0
		
		try:
			OpenMayaRender.MRenderView.startRender( tgaWidth, tgaHeight, 0)
		except:
			OpenMaya.MGlobal.displayError( "Girlview: error in startRender\n" )
			self.m_bLock = 0
			raise
		
		while (n < tgaWidth*tgaHeight):
		
			if tgaDataTypeCode == 2:
				try:
					p = fptr.read(byteCount)
				except:
					fptr.close()
					OpenMaya.MGlobal.displayError( "Unexpected EOF\n" )
					self.m_bLock = 0
					raise
				
				self.setPixel(tgaWidth, tgaHeight, n, ord(p[2]), ord(p[1]), ord(p[0]), ord(p[3]), byteCount)
				n += 1
				
			elif tgaDateTypeCode == 10:
				try:
					p = fptr.read(byteCount+1)
				except:
					fptr.close()
					OpenMaya.MGlobal.displayError( "Unexpected EOF\n" )
					self.m_bLock = 0
					raise
					
				j = ord(p[0]) & 0x7F
				
				self.setPixel(tgaWidth, tgaHeight, n, ord(p[3]), ord(p[2]), ord(p[1]), ord(p[4]), byteCount)
				n += 1
				
				if (p[0] & 0x80):	# RLE
					for i in range (0, j):
						self.setPixel(tgaWidth, tgaHeight, n, ord(p[3]), ord(p[2]), ord(p[1]), ord(p[4]), byteCount)
						n += 1
						
				else:				# Normal
					for i in range(0, j):
						try:
							p = fptr.read(byteCount)
						except:
							fptr.close()
							OpenMaya.MGlobal.displayError( "Unexpected EOF\n" )
							self.m_bLock = 0
							raise
						
						self.setPixel(tgaWidth, tgaHeight, n, ord(p[3]), ord(p[2]), ord(p[1]), ord(p[4]), byteCount)
						n += 1
			
		fptr.close()
		# End of file read
		
		try:
			OpenMayaRender.MRenderView.refresh(0, tgaWidth-1, 0, tgaHeight-1 )
		except:
			OpenMaya.MGlobal.displayError( "Girlview: error in refresh\n" )
			self.m_bLock = 0
			raise
		
		try:
			OpenMayaRender.MRenderView.endRender()
		except:
			OpenMaya.MGlobal.displayError( "Girlview: error in endRender\n" )
			self.m_bLock = 0
			raise
		
		self.m_bLock = 0
	# end def loadTgaIntoRenderView()
		
	def setPixel(self, width, height, n, r, g, b, a, bc):
		px = OpenMayaRender.RV_PIXEL()
		x = int(n % width)
		y = int(math.floor( n / width ))
		px.r = r
		px.g = g
		px.b = b
		if bc == 4:
			px.a = a
		else:
			px.a = 255
			
		OpenMayaRender.MRenderView.updatePixels(x, x, y, y, px )
	#end def setPixel()

		
	def dprint(self, str):
		if (self.m_bDebug):
			print str
	# end def dprint()