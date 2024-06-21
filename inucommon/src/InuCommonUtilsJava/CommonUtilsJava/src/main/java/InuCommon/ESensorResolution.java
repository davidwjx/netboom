package InuCommon;

import java.util.HashMap;
import java.util.Map;

public enum ESensorResolution {
    eUnknownResolution(0),
    eBinning(1),    
    eVerticalBinning(2),  
    eFull(3);

    
	ESensorResolution(int code)
	{
		this.mCode = code;
	}

	private static final Map<Integer, ESensorResolution> typesByValue = new HashMap<Integer, ESensorResolution>();

    static {
        for (ESensorResolution type : ESensorResolution.values()) {
            typesByValue.put(type.mCode, type);
        }
    }

    public static ESensorResolution forValue(int value) {
        return typesByValue.get(value);
    }

	private int mCode;	
}
