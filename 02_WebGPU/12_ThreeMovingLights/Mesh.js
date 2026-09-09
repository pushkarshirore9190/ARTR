// functions use without 'this' prefix are private to class.
// functions with 'this' prefix are 'public' and hence when they use
function Mesh()
{
    var elements=[];
    var verts=[];
    var norms=[];
    var texCoords=[];
    
    var numElements=0;
    var maxElements=0;
    var numVertices=0;
    var iNumIndices=0;

    this.allocate=function(numIndices)
    {
        // code
        // first cleanup, if not initially empty
        cleanupMeshData();
        
        maxElements = numIndices;
        numElements = 0;
        numVertices = 0;
        
        iNumIndices=numIndices/3;

        elements = new Uint16Array(iNumIndices * 3 * Uint16Array.BYTES_PER_ELEMENT); // 3 is x,y,z and 2 is sizeof short
        verts = new Float32Array(iNumIndices * 3 * Float32Array.BYTES_PER_ELEMENT); // 3 is x,y,z and 4 is sizeof float
        norms = new Float32Array(iNumIndices * 3 * Float32Array.BYTES_PER_ELEMENT); // 3 is x,y,z and 4 is sizeof float
        texCoords = new Float32Array(iNumIndices * 2 * Float32Array.BYTES_PER_ELEMENT); // 2 is s,t and 4 is sizeof float
    }

    // Add 3 vertices, 3 normal and 2 texcoords i.e. one triangle to the geometry.
    // This searches the current list for identical vertices (exactly or nearly) and
    // if one is found, it is added to the index array.
    // if not, it is added to both the index array and the vertex array.
    this.addTriangle=function(single_vertex, single_normal, single_texture)
    {
        //variable declarations
        const diff = 0.00001;
        var i, j;
        // code
        // normals should be of unit length
        normalizeVector(single_normal[0]);
        normalizeVector(single_normal[1]);
        normalizeVector(single_normal[2]);
        
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < numVertices; j++) //for the first ever iteration of 'j', numVertices will be 0 because of it's initialization in the parameterized constructor
            {
                if (isFoundIdentical(verts[j * 3], single_vertex[i][0], diff) &&
                    isFoundIdentical(verts[(j * 3) + 1], single_vertex[i][1], diff) &&
                    isFoundIdentical(verts[(j * 3) + 2], single_vertex[i][2], diff) &&
                    
                    isFoundIdentical(norms[j * 3], single_normal[i][0], diff) &&
                    isFoundIdentical(norms[(j * 3) + 1], single_normal[i][1], diff) &&
                    isFoundIdentical(norms[(j * 3) + 2], single_normal[i][2], diff) &&
                    
                    isFoundIdentical(texCoords[j * 2], single_texture[i][0], diff) &&
                    isFoundIdentical(texCoords[(j * 2) + 1], single_texture[i][1], diff))
                {
                    elements[numElements] = j;
                    numElements++;
                    break;
                }
            }
            
            //If the single vertex, normal and texture do not match with the given, then add the corressponding triangle to the end of the list
            if (j == numVertices && numVertices < maxElements && numElements < maxElements)
            {
                verts[numVertices * 3] = single_vertex[i][0];
                verts[(numVertices * 3) + 1] = single_vertex[i][1];
                verts[(numVertices * 3) + 2] = single_vertex[i][2];
                
                norms[numVertices * 3] = single_normal[i][0];
                norms[(numVertices * 3) + 1] = single_normal[i][1];
                norms[(numVertices * 3) + 2] = single_normal[i][2];
                
                texCoords[numVertices * 2] = single_texture[i][0];
                texCoords[(numVertices * 2) + 1] = single_texture[i][1];
                
                elements[numElements] = numVertices; //adding the index to the end of the list of elements/indices
                numElements++; //incrementing the 'end' of the list
                numVertices++; //incrementing coun of vertices
            }
        }
    }
    
    this.getMeshData=function()
    {
        // code
        // Float32Array.BYTES_PER_ELEMENT is sizeof float and 3 is x,y,z
        var vertices = new Float32Array(Float32Array.BYTES_PER_ELEMENT * 3 * iNumIndices);
        vertices = verts.slice();

        // Float32Array.BYTES_PER_ELEMENT is sizeof float and 3 is x,y,z
        var normals = new Float32Array(Float32Array.BYTES_PER_ELEMENT * 3 * iNumIndices); // 4 is sizeof float and 3 is x,y,z
        normals = norms.slice();

        // Float32Array.BYTES_PER_ELEMENT is sizeof float and 2 is s,t
        var textures = new Float32Array(Float32Array.BYTES_PER_ELEMENT * 2 * iNumIndices); // 4 is sizeof float and 2 is s,t
        textures = texCoords.slice();
        
        // Uint16Array.BYTES_PER_ELEMENT is sizeof float and 3 is x,y,z
        var indices = new Uint16Array(Uint16Array.BYTES_PER_ELEMENT * 3 * iNumIndices); // 2 is sizeof short and 3 is x,y,z
        indices = elements.slice();
        
        return({
                    verticesArray:vertices,
                    normalsArray:normals,
                    texCoordsArray:textures,
                    indicesArray:indices
               }); // named return values as part of returned object
    }
    
    this.getIndexCount=function()
    {
        // code
        return(numElements);
    }
    
    this.getVertexCount=function()
    {
        // code
        return(numVertices);
    }
    
    normalizeVector=function(v)
    {
        // code
        
        // square the vector length
        var squaredVectorLength=(v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]);
        
        // get square root of above 'squared vector length'
        var squareRootOfSquaredVectorLength=Math.sqrt(squaredVectorLength);
        
        // scale the vector with 1/squareRootOfSquaredVectorLength
        v[0] = v[0] * 1.0/squareRootOfSquaredVectorLength;
        v[1] = v[1] * 1.0/squareRootOfSquaredVectorLength;
        v[2] = v[2] * 1.0/squareRootOfSquaredVectorLength;
    }
    
    isFoundIdentical=function(val1, val2, diff)
    {
        // code
        if(Math.abs(val1 - val2) < diff)
            return(true);
        else
            return(false);
    }
    
    cleanupMeshData=function()
    {
        // code
        if(elements!=null)
        {
            elements=null;
        }
        
        if(verts!=null)
        {
            verts=null;
        }
        
        if(norms!=null)
        {
            norms=null;
        }
        
        if(texCoords!=null)
        {
            texCoords=null;
        }
    }
}



