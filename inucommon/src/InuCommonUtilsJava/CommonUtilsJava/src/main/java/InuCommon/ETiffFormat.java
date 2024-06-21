package InuCommon;

import java.util.HashMap;
import java.util.Map;

public enum ETiffFormat {
    eEmpty(0),			// Empty buffer      
    eDepth(1),			// Depth      
    eRGB565(5),         // Standard RGB565 format (2 bytes per pixel)  
    eYUV(100),
    eY(101),            // 10 bit Y 
    eBayerGRBG(104), 
    eNumOfFormats(105);
    

	ETiffFormat(int code)
	{
		this.mCode = code;
	}

	private static final Map<Integer, ETiffFormat> typesByValue = new HashMap<Integer, ETiffFormat>();

    static {
        for (ETiffFormat type : ETiffFormat.values()) {
            typesByValue.put(type.mCode, type);
        }
    }

    public static ETiffFormat forValue(int value) {
        return typesByValue.get(value);
    }

	private int mCode;	
}
