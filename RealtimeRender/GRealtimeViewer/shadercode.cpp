float testflag = loadV();
float cal_dep;
bool shaflag = false;

if(shadowflag == 1)
{
	bool flag = false;
	vec4 v_p = vec4(V,1.0);
	vec4 l_p = shamat*v_p;
	vec2 l_codi;
	l_codi[0] = l_p.x/l_p.w;
	l_codi[1] = l_p.y/l_p.w;
	float l_dep = l_p.z/l_p.w;

	l_codi[0] = (l_codi[0] + 1.0) * 0.5;
	l_codi[1] = (l_codi[1] + 1.0) * 0.5;

	if(l_codi[0] <0.0 || l_codi[0] >1.0)
	{
		flag = true;
	}
	if(l_codi[1] <0.0 || l_codi[1] >1.0)
	{
		flag = true;
	}

	if(!flag)
	{
		vec4 read_depth = texture2D(shadowmap, l_codi);
		cal_dep = read_depth[0];

		cal_dep = (cal_dep*2.0)-1.0;
		if(cal_dep > 0.9999999)
		{
			shaflag = false;
		}
		else if(l_dep > 0.9999999)
		{
			shaflag = false;
		}
		else if(cal_dep < (l_dep)-0.00005 && lightflag ==3)
		{
			shaflag = true;
		}
		else if(cal_dep < (l_dep)-0.005 && lightflag ==2)
		{
			shaflag = true;
		}
		else
		{
			shaflag = false;
		}
	}
}
else
{
	shaflag = false;
}
if(shaflag)
{
	gl_FragColor = vec4(0.0,0.0,0.0,alpha);					
}
else
{
	N = normalize(vec3(texture2D(normal, gl_TexCoord[0].st)*2.0)-1.0);

	bool lightoff = false;

	float dis;
	float powout = 1.0;
	float tmp_spot = 1.0;
	if(lightflag ==1)
	{
		L = normalize(LightPos - V);

		dis = distance(LightPos,V);
		powout = 1.0/pow(dis,float(decay));

	}
	else if(lightflag ==2)
	{
		L = LightDir;
		powout = 1.0;
	}
	else if(lightflag ==3)
	{
		L = normalize(LightPos - V);

		dis = distance(LightPos,V);
		powout = 1.0/pow(dis,float(decay));

		float co = dot(L,LightDir);
		if(co < cosPenumbra)
		{
			lightoff = true;
			gl_FragColor = vec4(0.0,0.0,0.0,alpha);
		}
		else
		{
			tmp_spot = pow(co,dropoff);
			if(co < cosUmbra)
			{
				tmp_spot = tmp_spot*(co - cosPenumbra)/(cosUmbra-cosPenumbra);
			}
			powout = powout * tmp_spot;
		}
	}
	else if(lightflag == 0)
	{
		float ambientShade = 0.0;
		lightoff = true;
		L = normalize(LightPos - V);
		NL = dot(N,L);
		ambientShade = AS*(NL-1.0);
		vec3 tmp_ambi;
		tmp_ambi = vec3(ambientShade+1.0,ambientShade+1.0,ambientShade+1.0);
		tmp_ambi = tmp_ambi*LightColor*getcolor();
		gl_FragColor = vec4(tmp_ambi,alpha);
	}
	else
	{
		lightoff = true;
	}
	if(powout >1.0)
	{
		powout = 1.0;
	}
	LightOut = LightColor*powout;

	if(!lightoff)
	{
		V = normalize(-V);
		NL = dot(N,L);
		if(NL <0.0)
		{
			NL = 0.0;
		}

		vec3 lambert = getcolor()*LightOut*vec3(getdiffuse(),getdiffuse(),getdiffuse())*NL;
		int chan = gettype();
		vec3 tmp_spec;
		if(chan == 3)
		{
			if(NL <0.0001)
			{
				gl_FragColor = vec4(lambert,alpha);
			}
			else
			{
				tmp_spec = phong_spec();
				gl_FragColor = vec4(lambert+tmp_spec, alpha);
				if(mulflag == 1)
				{
					gl_FragColor = vec4(lambert+tmp_spec, alpha)*texture2D(add, gl_TexCoord[0].st)*addIntensity;
				}
			}
		}
		else if(chan == 2)
		{
			if(NL <0.0001)
			{
				gl_FragColor = vec4(lambert,alpha);
			}
			else
			{
				tmp_spec = blinn_spec();
				gl_FragColor = vec4(lambert+tmp_spec, alpha);
				if(mulflag == 1)
				{
					gl_FragColor = vec4(lambert+tmp_spec, alpha)*texture2D(add, gl_TexCoord[0].st)*addIntensity;
				}
			}
		}
		else
		{
			if(NL <0.0001)
			{
				NL = 0.0;
			}
			else
			{
				gl_FragColor = vec4(lambert,alpha);
				if(mulflag == 1)
				{
					gl_FragColor = vec4(lambert, alpha)*texture2D(add, gl_TexCoord[0].st)*addIntensity;
				}
			}
		}
	}
}