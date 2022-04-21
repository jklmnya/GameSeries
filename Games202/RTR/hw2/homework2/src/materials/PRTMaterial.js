class PRTMaterial extends Material {

    constructor(LightSHRGB, vertexShader, fragmentShader) {
        super({
            'uSH[0]' : { type: '3fv', value : LightSHRGB[0] },
            'uSH[1]' : { type: '3fv', value : LightSHRGB[1] },
            'uSH[2]' : { type: '3fv', value : LightSHRGB[2] },
            'uSH[3]' : { type: '3fv', value : LightSHRGB[3] },
            'uSH[4]' : { type: '3fv', value : LightSHRGB[4] },
            'uSH[5]' : { type: '3fv', value : LightSHRGB[5] },
            'uSH[6]' : { type: '3fv', value : LightSHRGB[6] },
            'uSH[7]' : { type: '3fv', value : LightSHRGB[7] },
            'uSH[8]' : { type: '3fv', value : LightSHRGB[8] },
        }, ['aPrecomputeLT'], vertexShader, fragmentShader, null);
    }
    
}

async function buildPRTMaterial(LightSHRGB, vertexPath, fragmentPath) {
    let vertexShader = await getShaderString(vertexPath);
    let fragmentShader = await getShaderString(fragmentPath);
    return new PRTMaterial(LightSHRGB, vertexShader, fragmentShader);
}