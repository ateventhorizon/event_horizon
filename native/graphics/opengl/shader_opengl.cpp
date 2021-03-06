#include <iostream>
#include "shader_opengl.h"
#include "core/string_util.h"

GLuint Shader::compile() {
	GLenum glShaderType = GL_VERTEX_SHADER;

	glShaderType = getShaderType();
	if ( mHandle != 0 ) {
		GLCALL( glDeleteShader(mHandle) );
	}
	GLCALLRET( mHandle, glCreateShader( glShaderType ) );

	LOGI( "Compiling %s shader -- handle=%d", mId.c_str(), mHandle );
	//LOGI("Compiling %s shader (%s) handle=%d", mType == TYPE_VERTEX_SHADER ? "vertex" : "fragment", mId.c_str(), mHandle);

	if ( mHandle > 0 ) {
		const char* source = mSource.c_str();
		GLCALL( glShaderSource( mHandle, 1, &source, NULL ) );
		GLCALL( glCompileShader( mHandle ) );
		GLint compiled = 0;
		GLCALL( glGetShaderiv( mHandle, GL_COMPILE_STATUS, &compiled ) );

		char infoBuffer[2048];
		GLsizei length = 0;

		GLCALL( glGetShaderInfoLog( mHandle, sizeof( infoBuffer ), &length, infoBuffer ) );

		if ( compiled ) {
			if ( length > 0 ) {
				LOGW( "%s - %s", mId.c_str(), infoBuffer );
			}
            return mHandle;
		} else {
			LOGE( "Could not compile shader %s\n%s\n", mId.c_str(), infoBuffer );
			SHADER_ERROR( "Could not compile shader", mId.c_str(), infoBuffer );

			std::string ibs(infoBuffer);
			std::replace( ibs.begin(), ibs.end(), ':', ' ');
			char errStr[1024];
			int el, ec = 0;
			sscanf( ibs.c_str(), "%s %d %d:", errStr, &ec, &el );
			auto lines = split( source, '\n');
			int lineNumber = 0;
			for ( const auto& l : lines ) {
				if ( lineNumber == el ) {
					std::cout << "\033[1;31m" << lineNumber << " " << l << "\033[0m" << std::endl;
				}
   			    else {
					std::cout << lineNumber << " " << l << std::endl;
				}
				++lineNumber;
			}

			std::cout << infoBuffer << std::endl;

			mHandle = 0;
			return 0;
		}
	} else {
		LOGE( "Cound not create shader: %s", mId.c_str() );
		return 0;
	}

}

void Shader::setPreprocessDefine( const std::string& _define, const std::string& _value ) {
    // Inject global macros

    size_t fi = 0;
    std::istringstream smStream(mSource);
    size_t numCharRead = 0;
    auto defineStr = "#define " + _define;
    for (std::string line; std::getline(smStream, line); ) {
        numCharRead += line.size();
        if (( fi = line.find( defineStr )) != std::string::npos ) {
            size_t fe = line.find( "//" );
            if ( fe == std::string::npos || fe > fi ) {
                fi = mSource.find( defineStr, numCharRead-line.size() );
                fe = mSource.find( '\n', numCharRead );
                mSource.erase( mSource.begin() + fi, mSource.begin() + fe );
                mSource.insert( fi, defineStr + " " + _value );
            }
        }
    }

}
