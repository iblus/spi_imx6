/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.blus.SpiClient;
import com.blus.server.SpiService;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import java.lang.String;
public class SpiClient extends Activity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Call an API on the library.
		SpiService ls = new SpiService();
		//ls.setOn(1);
	        
        TextView tv = new TextView(this);
		TextView tv2 = new TextView(this);
		if(ls.open(1))
        tv.setText("SPI seccuss");
		else
		tv.setText("SPI fail");
        setContentView(tv);
    }
}

