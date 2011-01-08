#include <powder.h>

int update_SPRK(UPDATE_FUNC_ARGS) {
	update_PYRO(UPDATE_FUNC_SUBCALL_ARGS);
	int r, rt, conduct_sprk, nearp, pavg, ct = parts[i].ctype;
	if (parts[i].life<=0)
	{
		parts[i].type = ct;
		if (ct!=PT_METL&&ct!=PT_PTCT&&ct!=PT_NTCT&&ct!=PT_IRON&&ct!=PT_BMTL&&ct!=PT_BRMT&&ct!=PT_LRBD&&ct!=PT_RBDM&&ct!=PT_BTRY&&ct!=PT_NBLE&&ct!=PT_QRTZ)
			parts[i].temp = R_TEMP + 273.15f;
		if (!ct)
			ct = PT_METL;
		parts[i].type = ct;
		parts[i].life = 4;
		if (ct == PT_WATR)
			parts[i].life = 64;
		if (ct == PT_SLTW)
			parts[i].life = 54;
		if (ct == PT_SWCH)
			parts[i].life = 14;
		return 0;
	}
	if (ct==PT_SPRK)
	{
		kill_part(i);
		return 1;
	}
	else if (ct==PT_ETRD&&parts[i].life==1)
	{
		nearp = nearest_part(i, PT_ETRD);
		if (nearp!=-1&&parts_avg(i, nearp, PT_INSL)!=PT_INSL)
		{
			create_line((int)parts[i].x, (int)parts[i].y, (int)parts[nearp].x, (int)parts[nearp].y, 0, 0, PT_PLSM);
			parts[i].type = PT_ETRD;
			ct = parts[i].ctype = PT_NONE;
			parts[i].life = 20;
			parts[nearp].type = PT_SPRK;
			parts[nearp].life = 9;
			parts[nearp].ctype = PT_ETRD;
		}
	}
	else if (ct==PT_NBLE&&parts[i].life<=1)
	{
		parts[i].life = rand()%150+50;
		parts[i].type = PT_PLSM;
		parts[i].ctype = PT_NBLE;
		parts[i].temp = 3500;
		pv[y/CELL][x/CELL] += 1;
	}
	else if (ct==PT_IRON) {
		for (nx=-1; nx<2; nx++)
			for (ny=-1; ny<2; ny++)
				if (x+nx>=0 && y+ny>0 && x+nx<XRES && y+ny<YRES && (nx || ny))
				{
					r = pmap[y+ny][x+nx];
					if ((r>>8)>=NPART || !r)
						continue;
					if (((r&0xFF) == PT_DSTW && 30>(rand()/(RAND_MAX/1000))) ||
					        ((r&0xFF) == PT_SLTW && 30>(rand()/(RAND_MAX/1000))) ||
					        ((r&0xFF) == PT_WATR && 30>(rand()/(RAND_MAX/1000))))
					{
						parts[r>>8].type=PT_O2;
						//parts[r>>8].tmp=(rand()/(RAND_MAX/10))+20;
					}
				}
	}
	for (nx=-2; nx<3; nx++)
		for (ny=-2; ny<3; ny++)
			if (x+nx>=0 && y+ny>0 && x+nx<XRES && y+ny<YRES && (nx || ny))
			{
				r = pmap[y+ny][x+nx];
				if ((r>>8)>=NPART || !r)
					continue;
				rt = parts[r>>8].type;
				conduct_sprk = 1;


				pavg = parts_avg(r>>8, i,PT_INSL);
				if ((rt==PT_SWCH||(rt==PT_SPRK&&parts[r>>8].ctype==PT_SWCH)) && pavg!=PT_INSL) // make sparked SWCH turn off correctly
				{
					if (rt==PT_SWCH&&ct==PT_PSCN&&parts[r>>8].life<10) {
						parts[r>>8].life = 10;
					}
					if (ct==PT_NSCN) {
						parts[r>>8].type = PT_SWCH;
						parts[r>>8].ctype = PT_NONE;
						parts[r>>8].life = 9;
					}
				}
				else if ((ct==PT_PSCN||ct==PT_NSCN) && (rt==PT_PUMP||rt==PT_HSWC||(rt==PT_LCRY&&abs(nx)<2&&abs(ny)<2)))
				{
					if (ct==PT_PSCN) parts[r>>8].life = 10;
					else if (ct==PT_NSCN) parts[r>>8].life = 9;
				}


				// ct = spark from material, rt = spark to material. Make conduct_sprk = 0 if conduction not allowed

				if (pavg == PT_INSL) conduct_sprk = 0;
				if (!(ptypes[rt].properties&PROP_CONDUCTS||rt==PT_INST)) conduct_sprk = 0;
				if (abs(nx)+abs(ny)>=4 &&ct!=PT_SWCH&&rt!=PT_SWCH)
					conduct_sprk = 0;


				if (ct==PT_METL && (rt==PT_NTCT||rt==PT_PTCT||rt==PT_INWR||(rt==PT_SPRK&&(parts[r>>8].ctype==PT_NTCT||parts[r>>8].ctype==PT_PTCT))))
				{
					parts[r>>8].temp = 473.0f;
					if (rt==PT_NTCT||rt==PT_PTCT)
						conduct_sprk = 0;
				}
				if (ct==PT_NTCT && !(rt==PT_PSCN || rt==PT_NTCT || (rt==PT_NSCN&&parts[i].temp>373.0f)))
					conduct_sprk = 0;
				if (ct==PT_PTCT && !(rt==PT_PSCN || rt==PT_PTCT || (rt==PT_NSCN&&parts[i].temp<373.0f)))
					conduct_sprk = 0;
				if (ct==PT_INWR && !(rt==PT_NSCN || rt==PT_INWR || rt==PT_PSCN))
					conduct_sprk = 0;
				if (ct==PT_NSCN && rt==PT_PSCN)
					conduct_sprk = 0;
				if (ct==PT_ETRD && (parts[i].life!=5||!(rt==PT_METL||rt==PT_ETRD||rt==PT_BMTL||rt==PT_BRMT||rt==PT_LRBD||rt==PT_RBDM||rt==PT_PSCN||rt==PT_NSCN)))
					conduct_sprk = 0;
				if (ct==PT_INST&&rt!=PT_NSCN) conduct_sprk = 0;
				if (ct==PT_SWCH && (rt==PT_PSCN||rt==PT_NSCN||rt==PT_WATR||rt==PT_SLTW||rt==PT_NTCT||rt==PT_PTCT||rt==PT_INWR))
					conduct_sprk = 0;
				if (rt==PT_QRTZ && !((ct==PT_NSCN||ct==PT_METL||ct==PT_PSCN||ct==PT_QRTZ) && (parts[r>>8].temp<173.15||pv[(y+ny)/CELL][(x+nx)/CELL]>8)))
					conduct_sprk = 0;
				if (rt==PT_NTCT && !(ct==PT_NSCN || ct==PT_NTCT || (ct==PT_PSCN&&parts[r>>8].temp>373.0f)))
					conduct_sprk = 0;
				if (rt==PT_PTCT && !(ct==PT_NSCN || ct==PT_PTCT || (ct==PT_PSCN&&parts[r>>8].temp<373.0f)))
					conduct_sprk = 0;
				if (rt==PT_INWR && !(ct==PT_NSCN || ct==PT_INWR || ct==PT_PSCN))
					conduct_sprk = 0;
				if (rt==PT_INST&&ct!=PT_PSCN)
					conduct_sprk = 0;

				if (conduct_sprk) {
					if (ct==PT_ETRD) {
						parts[i].type = PT_ETRD;
						parts[i].ctype = PT_NONE;
						parts[i].life = 20;
						parts[r>>8].type = PT_SPRK;
						parts[r>>8].life = 4;
						parts[r>>8].ctype = rt;
					}
					else if (rt==PT_WATR||rt==PT_SLTW) {
						if (parts[r>>8].life==0 && (parts[i].life<2 || ((r>>8)<i && parts[i].life<3)))
						{
							parts[r>>8].type = PT_SPRK;
							if (rt==PT_WATR) parts[r>>8].life = 6;
							else parts[r>>8].life = 5;
							parts[r>>8].ctype = rt;
						}
					}
					else if (rt==PT_INST) {
						if (parts[i].life>=3&&parts[r>>8].life==0)
						{
							flood_parts(x+nx,y+ny,PT_SPRK,PT_INST,-1);//spark the wire
						}
					}
					else if (parts[r>>8].life==0 && (parts[i].life<3 || ((r>>8)<i && parts[i].life<4))) {
						parts[r>>8].life = 4;
						parts[r>>8].ctype = rt;
						rt = parts[r>>8].type = PT_SPRK;
						if (parts[r>>8].temp+10.0f<673.0f&&!legacy_enable&&(rt==PT_METL||rt==PT_BMTL||rt==PT_BRMT||rt==PT_PSCN||rt==PT_NSCN||rt==PT_ETRD||rt==PT_NBLE||rt==PT_IRON))
							parts[r>>8].temp = parts[r>>8].temp+10.0f;
					}
				}
			}
	return 0;
}
