import pymel.core as pm
import mtoa.utils as utils
import mtoa.ui.ae.utils as aeUtils
from mtoa.ui.ae.shaderTemplate import ShaderAETemplate
 
class AEmySimpleTemplate(ShaderAETemplate):

    def setup(self):
        # Add the shader swatch to the AE
        self.addSwatch()
        self.beginScrollLayout()
 
        # Add a list that allows to replace the shader for other one
        self.addCustom('message', 'AEshaderTypeNew',
        'AEshaderTypeReplace')
 
        # Begins a "Color Section"
        self.beginLayout("Color Section", collapse=False)

        # Add a control for the "constatColor" shader attribute
        self.addControl("Kd_color", label="Diffuse Color",
        annotation="Diffuse Color")
 
        # Begins a "Color Section"
        self.beginLayout("Specular Section", collapse=False)
 
        self.addControl("Ks_color", label="Specular Color",
        annotation="Specular Color")

        self.addControl("roughness", label="Roughness",
        annotation="Roughness")

        self.addControl("specbalance", label="Specbalance",
        annotation="Specbalance")

        self.endLayout()
 
        # include/call base class/node attributes
        pm.mel.AEdependNodeTemplate(self.nodeName)
 
        # Add Section for the extra controls not displayed before
        self.addExtraControls()
        self.endScrollLayout()