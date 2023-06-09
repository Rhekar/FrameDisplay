// .HA6 loader:
//
// Hantei6 frame data load and storage.

#include "mbtl_framedisplay.h"

#include <list>

#include <cstring>
struct TempInfo {
	MBTL_Sequence	*seq;

	unsigned int	cur_hitbox;
	unsigned int	cur_AT;
	unsigned int	cur_AS;
	unsigned int	cur_EF;
	unsigned int	cur_IF;
};
/*
struct TempInfo {
    struct DelayLoad
    {
        unsigned int frameNo;
        unsigned int location;
        unsigned int source;
    };
    Sequence	*seq;
    std::vector<Hitbox*> boxesRefs;
    std::vector<DelayLoad> delayLoadList;
    unsigned int cur_hitbox;
    unsigned int cur_AS;
    unsigned int cur_frame;

    std::vector<Frame_AS*> AS;
};
*/

// local recursive frame data loader

static unsigned int *fd_frame_AT_load(unsigned int *data, const unsigned int *data_end,
				MBTL_Frame_AT *AT) {
	AT->active = 1;
	AT->guard_flags = 0;
	AT->proration = 0;
	AT->proration_type = 0;
	AT->damage = 0;
	AT->red_damage = 0;
	AT->circuit_gain = 0;

	while (data < data_end) {
		unsigned int *buf = data;
		++data;

		if (!memcmp(buf, "ATGD", 4)) {
			AT->guard_flags = data[0];
			++data;
		} else if (!memcmp(buf, "ATV2", 4)) {
      //TODO: later:
      data += 12;
		} else if (!memcmp(buf, "ATHS", 4)) {
			AT->proration = data[0];
			++data;
		} else if (!memcmp(buf, "ATVV", 4)) {
			short *d = (short *)data;
			AT->red_damage = d[0];
			AT->damage = d[1];
			AT->dmg_unknown = d[2];
			AT->circuit_gain = d[3];
			data += 2;
		} else if (!memcmp(buf, "ATHT", 4)) {
			AT->proration_type = data[0];
			data += 1;
		} else if (!memcmp(buf, "ATHH", 4)) {
      //TODO: later:
      //AT->proration_type = data[0];
      data += 1;
		} else if (!memcmp(buf, "ATGV", 4)) {
			//First number can be different from 3. See CMHisui's 421C
			//Second byte is a flag. Extract it separately.
			// assert(0); //Melty only?
			// assert(data[0] <= 3);
			// for(int i = 0; i < data[0]; i++)
			// {
			// 	AT->guardVector[i] = data[i+1] & 0xFF;
			// 	AT->gVFlags[i] = data[i+1] >> 8;
			// 	//Only old ckohamech has buggy flag values.
			// }
			data += data[0]+1;
		} else if (!memcmp(buf, "ATHV", 4)) {
			// assert(0); //Melty only?
			// assert(data[0] <= 3);
			// for(int i = 0; i < data[0]; i++)
			// {
			// 	AT->hitVector[i] = data[i+1] & 0xFF;
			// 	AT->hVFlags[i] = data[i+1] >> 8;
			// }
			data += data[0]+1;
		} else if (!memcmp(buf, "ATF1", 4)) {
      // AT->otherFlags = data[0];

 			/* if(data[0] != 0)
			{
				test.Print(data, data_end);
				std::cout << "ATF1 "<< data[0]<<"\n";
			} */
			++data;
		} else if (!memcmp(buf, "ATAT", 4)) {
        //AT->damage = *data;
			data += 1;
		} else if (!memcmp(buf, "ATAM", 4)) {
        //AT->minDamage = *data;
			data += 1;
		} else if (!memcmp(buf, "ATCA", 4)) {
        //AT->meter_gain = *data;
			data += 1;
		} else if (!memcmp(buf, "ATC0", 4)) {
        //memcpy(AT->hitStunDecay, data, sizeof(int)*3);
			data += 3;
		} else if (!memcmp(buf, "ATHE", 4)) {
        //AT->hitEffect = data[0];
        //AT->soundEffect = data[1];
			data += 2;
		} else if (!memcmp(buf, "ATKK", 4)) {
        //AT->addedEffect = data[0];
			//Uni uses higher values.
			data++;
		} else if (!memcmp(buf, "ATNG", 4)) {
			//Melty only uses 1. UNI uses higher values.
			//AT->hitgrab = data[0];
			data++;
		} else if (!memcmp(buf, "ATUH", 4)) {
        //AT->extraGravity = ((float*)data)[0];
			data++;
		} else if (!memcmp(buf, "ATBT", 4)) {
        //AT->breakTime = data[0];
			data++;
		} else if (!memcmp(buf, "ATSN", 4)) {
        //AT->hitStopTime = data[0];
			data++;
		} else if (!memcmp(buf, "ATSU", 4)) {
        //AT->untechTime = data[0];
			data++;
		} else if (!memcmp(buf, "ATSP", 4)) {
        //AT->hitStop = data[0];
			data++;
		} else if (!memcmp(buf, "ATSA", 4)) {
        //AT->addHitStun = data[0];
			data++;
		} else if (!memcmp(buf, "ATSH", 4)) {
        //AT->hitStun = data[0];
			data++;
		} else if (!memcmp(buf, "ATGN", 4)) {
        //AT->blockStopTime = data[0];
			data++;
		} else if (!memcmp(buf, "ATED", 4)) {
			break;
		}
	}

	return data;
}

static unsigned int *fd_frame_AS_load(unsigned int *data, const unsigned int *data_end,
				MBTL_Frame_AS *AS) {
	AS->speed_flags = 0;
	AS->ASMV = -1;

	AS->stand_state = 0;

	AS->cancel_flags = 0;

	while (data < data_end) {
		unsigned int *buf = data;
		++data;

		if (!memcmp(buf, "ASV0", 4)) {
			int *d = (int *)data;
			AS->speed_flags = d[0];
			AS->speed_horz = d[1];
			AS->speed_vert = d[2];
			AS->accel_horz = d[3];
			AS->accel_vert = d[4];
			data += 5;
		} else if (!memcmp(buf, "ASVX", 4)) {
			AS->speed_flags = 0x33; // ?
			AS->speed_horz = 0;
			AS->speed_vert = 0;
			AS->accel_horz = 0;
			AS->accel_vert = 0;
		} else if (!memcmp(buf, "ASMV", 4)) {
			AS->ASMV = data[0];
			++data;
		} else if (!memcmp(buf, "ASS1", 4)) {
			AS->stand_state = 1; // airborne
		} else if (!memcmp(buf, "ASS2", 4)) {
			AS->stand_state = 2; // crouching
		} else if (!memcmp(buf, "ASCN", 4)) {
			AS->cancel_flags |= 1;
		} else if (!memcmp(buf, "ASCS", 4)) {
			AS->cancel_flags |= 2;
		} else if (!memcmp(buf, "ASCT", 4)) {
			AS->cancel_flags |= 4;
			/*
		} else if (!memcmp(buf, "AST0", 4)) {
			float *d = (float *)(data+5);
			printf("AST0: %d %d %d %d %d %1.10f %d\n",
				data[0], data[1], data[2], data[3],
				data[4], *d, data[6]);

			data += 7;
			 */
    } else if (!memcmp(buf, "ASCF", 4)) {
        //AS->ascf = data[0];
			data++;
		} else if (!memcmp(buf, "AST0", 4)) {
			// AS->sineFlags = data[0] & 0xFF; //Other values have no effect.
			// memcpy(AS->sineParameters, data+1, sizeof(int)*4);
			// AS->sinePhases[0] = ((float*)data)[5];
			// AS->sinePhases[1] = ((float*)data)[6];
			// if((data[0]&~0x11) != 0)
			// {
			// 	test.Print(data, data_end);
			// 	std::cout << "AST0 has nonstandard flags\n";
			// }
			data += 7;
		} else if (!memcmp(buf, "ASMX", 4)) {
        // AS->maxSpeedX = data[0];
			data++;
		} else if (!memcmp(buf, "ASAA", 4)) {
			// AS->hitsNumber = data[0];

			data++;
		} else if (!memcmp(buf, "ASYS", 4)) {
			// AS->invincibility = data[0];
			// if(data[0] > 4)
			// {
			// 	test.Print(data, data_end);
			// 	std::cout <<"\tUnknown ASYS value: " << data[0] <<"\n";
			// }
			data++;
		} else if (!memcmp(buf, "ASF", 3)) {
			// char t = ((char *)buf)[3];
			// if(t != '0' && t !='1')
			// {
			// 	test.Print(data, data_end);
			// 	std::cout <<"\tUnknown ASF suffix" << t <<"\n";
			// }
			// else
			// {
			// 	AS->statusFlags[t-'0'] = data[0];
			// }
			data++;
		} else if (!memcmp(buf, "ASED", 4)) {
			break;
		}

		// unhandled:
		// ASAA(1) - ???
		// ASMX(1) - ?
		// ASF0(1) - ?
		// ASF1(1) - ?
		// ASYS(1) - ?
		// AST0(7) - ??? Kohaku/Necos/Nero/Roa/Ryougi/Warakia only
		// (no others)
	}

	return data;
}

static unsigned int *fd_frame_EF_load(unsigned int *data, const unsigned int *data_end,
				MBTL_Frame_EF *EF) {
	while (data < data_end) {
		unsigned int *buf = data;
		++data;

		if (!memcmp(buf, "EFTP", 4)) {
			EF->command = data[0];
			++data;
		} else if (!memcmp(buf, "EFNO", 4)) {
			EF->parameter = data[0];
			++data;
		} else if (!memcmp(buf, "EFPR", 4)) {
			int count = data[0];
			if (count <= 12) {
				for (int i = 0; i < count; ++i) {
					EF->values[i] = data[i+1];
				}
			} else {
				//printf("EFPR %d ???\n",count);
			}
			data += count + 1;
		} else if (!memcmp(buf, "EFED", 4)) {
			break;
		}
	}

	return data;
}

static unsigned int *fd_frame_IF_load(unsigned int *data, const unsigned int *data_end,
				MBTL_Frame_IF *IF) {
	while (data < data_end) {
		unsigned int *buf = data;
		++data;

		if (!memcmp(buf, "IFTP", 4)) {
			IF->command = data[0];
			++data;
		} else if (!memcmp(buf, "IFPR", 4)) {
			int count = data[0];
			if (count <= 12) {
				for (int i = 0; i < count; ++i) {
					IF->values[i] = data[i+1];
				}
			} else {
				//printf("IFPR %d ???\n",count);
			}
			data += count + 1;
		} else if (!memcmp(buf, "IFED", 4)) {
			break;
		}
	}

	return data;
}
/*
static unsigned int *fd_frame_AF_load(unsigned int *data, const unsigned int *data_end,
				MBTL_Frame *frame) {
	frame->AF.active = 1;
	frame->AF.frame = -1;
	frame->AF.frame_unk = -1;
	frame->AF.offset_x = 0;
	frame->AF.offset_y = 8;
	frame->AF.duration = 1;
	frame->AF.AFF = -1;
	frame->AF.blend_mode = 0;
	frame->AF.alpha = 255;
	frame->AF.red = 255;
	frame->AF.green = 255;
	frame->AF.blue = 255;
	frame->AF.z_rotation = 0.0;
	frame->AF.x_rotation = 0.0;
	frame->AF.y_rotation = 0.0;
	frame->AF.has_zoom = 0;
	frame->AF.zoom_x = 1.0;
	frame->AF.zoom_y = 1.0;
	frame->AF.AFJP = -1;
  int spriteLayer = -1;
  int layerId = -1;

	while (data < data_end) {
		unsigned int *buf = data;
		++data;

		if (!memcmp(buf, "AFGP", 4)) { // Deprecated?
			int *dt = (int *)data;
			frame->AF.frame = dt[1];
			frame->AF.frame_unk = dt[0];
			data += 2;
    } else if (!memcmp(buf, "AFGX", 4)) {
      int *dt = (int *)data;
      //There can be multiple of these. Not gonna bother yet.
			layerId = dt[0];
			if(frame->AF.frame == -1 && dt[1] != 1)
      {
        frame->AF.frame = dt[2];
        //frame->AF.usePat = dt[1];
        spriteLayer = dt[0];
      }
			data += 3;
		} else if (!memcmp(buf, "AFOF", 4) && layerId == spriteLayer) {
			int *dt = (int *)data;
			frame->AF.offset_y = dt[1];
			frame->AF.offset_x = dt[0];
			data += 2;
		} else if (!memcmp(buf, "AFD", 3)) {
			char t = ((char *)buf)[3];
			if (t >= '0' && t <= '9') {
				frame->AF.duration = t - '0';
			} else if (t == 'L') {
				frame->AF.duration = data[0];
				++data;
			}
		} else if (!memcmp(buf, "AFY", 3)) {
			// yeah i don't know.
			// 7/8/9/X/1/2/3 -> 7/8/9/10/11/12/13
			// other numbers are not used.
			char t = ((char *)buf)[3];
			if (t >= '0' && t <= '9') {
				int v = (t - '0');
				if (v < 4) {
					v += 10;
				}
				frame->AF.offset_y = v;
			} else if (t == 'X') {
				frame->AF.offset_y = 10;
			}
		} else if (!memcmp(buf, "AFF", 3)) {
			char t = ((char *)buf)[3];
			if (t >= '0' && t <= '9') {
				frame->AF.AFF = t - '0';
			} else if (t == 'X') {
				frame->AF.AFF = data[0];
				++data;
			}
		} else if (!memcmp(buf, "AFAL", 4)) {
			frame->AF.blend_mode = data[0];
			frame->AF.alpha = data[1];
			data += 2;
		} else if (!memcmp(buf, "AFRG", 4)) {
			frame->AF.red = data[0];
			frame->AF.green = data[1];
			frame->AF.blue = data[2];
			data += 3;
		} else if (!memcmp(buf, "AFAZ", 4)) {
			frame->AF.z_rotation = *(float *)data;

			++data;
		} else if (!memcmp(buf, "AFAY", 4)) {
			frame->AF.y_rotation = *(float *)data;

			++data;
		} else if (!memcmp(buf, "AFAX", 4)) {
			frame->AF.x_rotation = *(float *)data;

			++data;
		} else if (!memcmp(buf, "AFZM", 4)) {
			frame->AF.has_zoom = 1;
			frame->AF.zoom_x = ((float *)data)[0];
			frame->AF.zoom_y = ((float *)data)[1];

			data += 2;
		} else if (!memcmp(buf, "AFJP", 4)) {
			frame->AF.AFJP = data[0];
			++data;
		} else if (!memcmp(buf, "AFED", 4)) {
			break;
		}

		// unhandled:
		// AFTN(2)
		// AFRT(1)
		// AFHK(1)
		// AFPR(1)
		// AFCT(1)
		// AFJP(1)
		// AFLP(1)
		// AFJC(1)
	}

	return data;
}
*/

unsigned int *fd_frame_AF_load2(unsigned int *data, const unsigned int *data_end, MBTL_Frame *frame)
{
	Layer *currentLayer = nullptr;
	frame->AF.active = 1;
	while (data < data_end) {
		unsigned int *buf = data;
		++data;
		
		if (!memcmp(buf, "AFGP", 4)) { //Melty graphics
			//assert(0 && "AFGP");
			/* int *dt = (int *)data;
			frame->AF.spriteId = dt[1];
			frame->AF.usePat = dt[0]; */
			data += 2;
		} else if (!memcmp(buf, "AFGX", 4)) { //UNI gfx
			//There can be multiple of these.
			int *dt = (int *)data;
			////assert(frame->AF.layers.size() == dt[0]);
			frame->AF.layers.push_back({});
			currentLayer = &frame->AF.layers.back();		
			currentLayer->spriteId = dt[2];
			currentLayer->usePat = dt[1];
			////assert(data[1] < 2);
			data += 3;
		} else if (!memcmp(buf, "AFOF", 4)) {
			int *dt = (int *)data;
			currentLayer->offset_y = dt[1];
			currentLayer->offset_x = dt[0];
			data += 2;
		} else if (!memcmp(buf, "AFPL", 4)) {
			currentLayer->priority = data[0];
			data += 1;
		} else if (!memcmp(buf, "AFAL", 4)) {
			currentLayer->blend_mode = data[0];
			currentLayer->rgba[3] = ((float)data[1])/255.f;
			//assert(data[0] >= 1 || data[0] <= 3 );
			data += 2;
		} else if (!memcmp(buf, "AFRG", 4)) {
			currentLayer->rgba[0] = ((float)data[0])/255.f;
			currentLayer->rgba[1] = ((float)data[1])/255.f;
			currentLayer->rgba[2] = ((float)data[2])/255.f;
			data += 3;
		} else if (!memcmp(buf, "AFAZ", 4)) {
			currentLayer->rotation[2] = *(float *)data;
			++data;
		} else if (!memcmp(buf, "AFAY", 4)) {
			currentLayer->rotation[1] = *(float *)data;
			++data;
		} else if (!memcmp(buf, "AFAX", 4)) {
			currentLayer->rotation[0] = *(float *)data;
			++data;
		} else if (!memcmp(buf, "AFZM", 4)) {
			currentLayer->scale[0] = ((float *)data)[0];
			currentLayer->scale[1] = ((float *)data)[1];
			data += 2;
		
		} else if (!memcmp(buf, "AFD", 3)) {
			char t = ((char *)buf)[3];
			if (t >= '0' && t <= '9') {
				frame->AF.duration = t - '0';
			} else if (t == 'L') {
				frame->AF.duration = data[0];
				++data;
			}
		} else if (!memcmp(buf, "AFY", 3)) { //Probably deprecated
			// 7/8/9/X/1/2/3 -> 7/8/9/10/11/12/13
			// Overrides AFOF
			currentLayer->offset_x = 0;
			char t = ((char *)buf)[3];
			if (t >= '0' && t <= '9') {
				int v = (t - '0');
				if (v < 4) {
					v += 10;
				}
				currentLayer->offset_y = v;
			} else if (t == 'X') {
				currentLayer->offset_y = 10;
			}
		} else if (!memcmp(buf, "AFF", 3)) {
			char t = ((char *)buf)[3];
			if (t >= '1' && t <= '2') {
				//Only values 1 and 2 are used.
				frame->AF.aniType = t - '0';
				//assert(t - '0' <= 2);
			} else if (t == 'L') {
				frame->AF.aniType = data[0];
				//assert(data[0] == 3);
				++data;
			} else if (t == 'E') {
				frame->AF.aniFlag = data[0];
				//assert(data[0] < 0x10); //Only lower four?
				++data;
			}
			else {
          //test.Print(data, data_end);
          //std::cout <<"\tAFF uses uknown value: " << t <<"\n";
			}
		} else if (!memcmp(buf, "AFJH", 4)) {
			frame->AF.afjh = data[0];
			//assert(data[0] == 1);
			data += 1;

		} else if (!memcmp(buf, "AFJP", 4)) {
			frame->AF.jump = data[0];
			++data;
		} else if (!memcmp(buf, "AFHK", 4)) {
			frame->AF.interpolationType = data[0];
			//assert(data[0] <= 5);
			++data;
		} else if (!memcmp(buf, "AFPA", 4)) {
			memcpy(frame->AF.param, data, sizeof(char)*4);
			++data;
		} else if (!memcmp(buf, "AFPR", 4)) {
			frame->AF.priority = data[0];
			++data;
		} else if (!memcmp(buf, "AFCT", 4)) {
			frame->AF.loopCount = data[0];
			++data;
		} else if (!memcmp(buf, "AFLP", 4)) {
			frame->AF.loopEnd = data[0];
			++data;
		} else if (!memcmp(buf, "AFJC", 4)) {
			frame->AF.landJump = data[0];
			++data;
		} else if (!memcmp(buf, "AFTN", 4)) {
			//Overrides rotation
			//assert(0); //Deprecated?
			currentLayer->rotation[0] = data[0] ? 0.5f : 0.f;
			currentLayer->rotation[1] = data[1] ? 0.5f : 0.f;
			data += 2;
		} else if (!memcmp(buf, "AFRT", 4)) {
			//Some fucked up interaction with rotation and scale order.
			frame->AF.AFRT = data[0];
			++data;
		} else if (!memcmp(buf, "AFID", 4)) {
			frame->AF.frameId = data[0];
			++data;
		} else if (!memcmp(buf, "AFED", 4)) {
			break;
		} else {
			char tag[5]{};
			memcpy(tag,buf,4);
			//test.Print(data, data_end);
			//std::cout <<"\tUnknown AF tag: " << tag <<"\n";
		}
		//Unhandled: None, unless they're not in vanilla melty files.
	}

	return data;
}





static unsigned int *fd_frame_load(unsigned int *data, const unsigned int *data_end,
				MBTL_Frame *frame, TempInfo *info) {
	memset(frame, 0, sizeof(MBTL_Frame));

	while (data < data_end) {
		unsigned int *buf = data;
		++data;

		if (!memcmp(buf, "HRNM", 4) || !memcmp(buf, "HRAT", 4)) {
			// read hitbox or attackbox
			unsigned int n = data[0];
			if (!memcmp(buf, "HRAT", 4)) {
				n += 25;
			}
			if (n <= 32 && info->cur_hitbox < info->seq->nhitboxes) {
				MBTL_Hitbox *d = &info->seq->hitboxes[info->cur_hitbox];
				++info->cur_hitbox;

				frame->hitboxes[n] = d;

				int *dt = (int *)data;

				d->x1 = (dt[1]);
				d->y1 = (dt[2]);
				d->x2 = (dt[3]);
				d->y2 = (dt[4]);
			}

			data += 5;
		} else if (!memcmp(buf, "HRNS", 4) || !memcmp(buf, "HRAS", 4)) {
			// read hitbox reference
			unsigned int location = data[0];
			unsigned int source = data[1];

			if (!memcmp(buf, "HRAS", 4)) {
				location += 25;
			}

			if (location <= 32 && source < info->cur_hitbox) {
				frame->hitboxes[location] = &info->seq->hitboxes[source];
			}

			data += 2;
		} else if (!memcmp(buf, "ATST", 4)) {
			// start attack block
			if (info->cur_AT < info->seq->nAT) {
				frame->AT = &info->seq->AT[info->cur_AT];
				++info->cur_AT;

				data = fd_frame_AT_load(data, data_end, frame->AT);
			}
		} else if (!memcmp(buf, "ASST", 4)) {
			// start state block
			if (info->cur_AS < info->seq->nAS) {
				frame->AS = &info->seq->AS[info->cur_AS];
				++info->cur_AS;

				data = fd_frame_AS_load(data, data_end, frame->AS);
			}
		} else if (!memcmp(buf, "ASSM", 4)) {
			// reference previous state block
			unsigned int value = data[0];
			++data;

			if (value < info->cur_AS) {
				frame->AS = &info->seq->AS[value];
			}
		} else if (!memcmp(buf, "AFST", 4)) {
			// start render frame block
			data = fd_frame_AF_load2(data, data_end, frame);
		} else if (!memcmp(buf, "EFST", 4)) {
			// start effect block
			int n = data[0];
			++data;
			if (n < 8 && info->cur_EF < info->seq->nEF) {
				frame->EF[n] = &info->seq->EF[info->cur_EF];
				++info->cur_EF;

				data = fd_frame_EF_load(data, data_end, frame->EF[n]);
			}
		} else if (!memcmp(buf, "IFST", 4)) {
			// start cancel block
			int n = data[0];
			++data;
			if (n < 8 && info->cur_IF < info->seq->nIF) {
				frame->IF[n] = &info->seq->IF[info->cur_IF];
				++info->cur_IF;

				data = fd_frame_IF_load(data, data_end, frame->IF[n]);
			}
		} else if (!memcmp(buf, "FEND", 4)) {
			break;
		}

		// unhandled:
		// FSNA(1) ?
		// FSNH(1) ?
		// FSNE(1) ?
		// FSNI(1) ?
		// (no others)
	}

	return data;
}

static unsigned int *fd_sequence_load(unsigned int *data, const unsigned int *data_end,
                                      MBTL_Sequence *seq, int seq_id) {
	TempInfo temp_info;
	bool initialized = 0;
	unsigned int frame_no = 0;

	temp_info.seq = seq;
	temp_info.cur_hitbox = 0;
	temp_info.cur_AT = 0;
	temp_info.cur_AS = 0;
	temp_info.cur_EF = 0;
	temp_info.cur_IF = 0;

	while (data < data_end) {
		unsigned int *buf = data;
		++data;

		if (!memcmp(buf, "PTCN", 4)) {
      unsigned int len = data[0];
			data += 1;

			char str[65];
			memcpy(str, data, len);
			str[len] = '\0';
			seq->name = str;
			data = (unsigned int *)(((unsigned char *)data)+len);
		} else if (!memcmp(buf, "PSTS", 4)) {
			++data;
		} else if (!memcmp(buf, "PLVL", 4)) {
			++data;
		} else if (!memcmp(buf, "PFLG", 4)) {
			++data;
		} else if (!memcmp(buf, "PDST", 4)) {
			// PDST is only used on G_CHAOS
			// seems to be an older format with vague
			// allocations. On-demand maybe.
			++data;
		} else if (!memcmp(buf, "PTT2", 4)) {
			// variable-length sequence title
			unsigned int len = data[0];
			if (len < 64) {
				char str[65];
				memcpy(str, data+1, len);
				str[len] = '\0';

				seq->name = str;
			}

			data += 1 + ((len+3)/4);
		} else if (!memcmp(buf, "PTIT", 4)) {
			// fixed-length sequence title
			char str[33];
			memcpy(str, data, 32);
			str[32] = '\0';

			seq->name = str;

			data += 8;
		} else if (!memcmp(buf, "PDST", 4)) {
			// old-style allocation

			// seems to alloc on demand as FSTR and whatnot
			// are called. this will require a parsing pass
			// that i don't care to do right now.

			// only G_CHAOS uses it and we don't really care
			// about him anyway. Don't even have any way
			// of rendering his shit!
		} else if (!memcmp(buf, "PDS2", 4)) {
			// this is an allocation call
			// format:
			// data[0] = byte count
			// data[1] = frame count
			// data[2] = hitbox count
			// data[3] = EF count
			// data[4] = IF count
			// data[5] = AT count
			// data[6] = unused
			// data[7] = AS count
			// from this we can tell what's referenced
			// and what's static for each frame. yay!
			if (data[0] == 32) {
				// trash old data
				if (seq->data) {
					delete[] seq->data;
				}

				seq->nframes = data[1];
				seq->nhitboxes = data[2];
				seq->nEF = data[3];
				seq->nIF = data[4];
				seq->nAT = data[5];
				seq->nAS = data[7];

				int size;
				size = sizeof(MBTL_Frame) * seq->nframes;
				size += sizeof(MBTL_Hitbox) * seq->nhitboxes;
				size += sizeof(MBTL_Frame_AT) * seq->nAT;
				size += sizeof(MBTL_Frame_AS) * seq->nAS;
				size += sizeof(MBTL_Frame_EF) * seq->nEF;
				size += sizeof(MBTL_Frame_IF) * seq->nIF;

				char *data = new char[size];
				seq->data = data;

				seq->frames = (MBTL_Frame *)data;
				size = sizeof(MBTL_Frame) * seq->nframes;

				seq->hitboxes = (MBTL_Hitbox *)(data + size);
				size += sizeof(MBTL_Hitbox) * seq->nhitboxes;

				seq->AT = (MBTL_Frame_AT *)(data + size);
				size += sizeof(MBTL_Frame_AT) * seq->nAT;

				seq->AS = (MBTL_Frame_AS *)(data + size);
				size += sizeof(MBTL_Frame_AS) * seq->nAS;

				seq->EF = (MBTL_Frame_EF *)(data + size);
				size += sizeof(MBTL_Frame_EF) * seq->nEF;

				seq->IF = (MBTL_Frame_IF *)(data + size);

				seq->subframe_count = 0;

				seq->initialized = 1;
				initialized = 1;
			}
			data += 1 + (data[0]/4);
		} else if (!memcmp(buf, "FSTR", 4)) {
			if (seq->initialized && frame_no < seq->nframes) {
				MBTL_Frame *frame = &seq->frames[frame_no];

				data = fd_frame_load(data, data_end, frame, &temp_info);

				++frame_no;

				seq->subframe_count += frame->AF.duration;
			}
		} else if (!memcmp(buf, "PEND", 4)) {
			break;
		} else if (!memcmp(buf, "PUPS", 4)) {
        // ?????
        ++data;
		} else {
      char tag[9]{};
      memcpy(tag,buf,8);
      FILE *fp2 = fopen ("error.txt", "w");
      fprintf( fp2, "%d seq Unknown Pattern level tag: %d %d %d %d %d %d %d %d", seq_id,
               tag[0],
               tag[1],
               tag[2],
               tag[3],
               tag[4],
               tag[5],
               tag[6],
               tag[7]
               );
      fclose( fp2 );
    }
	}

	if (initialized && seq->initialized && frame_no < seq->nframes) {
		seq->nframes = frame_no;
	}

	return data;
}

static unsigned int *fd_main_load(unsigned int *data, const unsigned int *data_end,
				MBTL_Sequence **sequences,
				unsigned int nsequences) {
	while (data < data_end) {
		unsigned int *buf = data;
		++data;

		if (!memcmp(buf, "PSTR", 4)) {
			unsigned int seq_id = *data;
			++data;

			// make sure there's actually something here.
			if (memcmp(data, "PEND", 4)) {
				if (seq_id < nsequences) {
					MBTL_Sequence *seq = sequences[seq_id];
					if (!seq) {
						seq = new MBTL_Sequence;

						sequences[seq_id] = seq;

						seq->subframe_count = 0;

						seq->initialized = 0;
						seq->data = 0;

						seq->is_move = 0;
						seq->move_meter = 0;
					}
					data = fd_sequence_load(data, data_end, seq, seq_id);
				}
			} else {
				++data;
			}
		} else if (!memcmp(buf, "_END", 4)) {
			break;
		}
	}

	return data;
}

bool MBTL_FrameData::load(MBTL_Pack *pack, const char *filename, int bsize, int offset) {
	// allow loading over existing data

	char *data;
	unsigned int size;

	if (!pack->read_file(filename, &data, &size, bsize, offset)) {
		return 0;
	}

	// verify header
	if (memcmp(data, "Hantei6DataFile", 15)) {
		delete[] data;

		return 0;
	}

	// initialize the root
	unsigned int *d = (unsigned int *)(data + 0x20);
	unsigned int *d_end = (unsigned int *)(data + size);
	if (memcmp(d, "_STR", 4)) {
		delete[] data;
		return 0;
	}

	unsigned int sequence_count = d[1];

	if (m_sequences) {
		if (sequence_count > m_nsequences) {
			MBTL_Sequence **seqs = new MBTL_Sequence*[sequence_count];
			for (unsigned int i = 0; i < m_nsequences; ++i) {
				seqs[i] = m_sequences[i];
			}
			for (unsigned int i = m_nsequences; i < sequence_count; ++i) {
				seqs[i] = 0;
			}
			delete[] m_sequences;
			m_sequences = seqs;
			m_nsequences = sequence_count;
		}
	} else {
		m_nsequences = sequence_count;
		m_sequences = new MBTL_Sequence*[m_nsequences];
		for (unsigned int i = 0; i < m_nsequences; ++i) {
			m_sequences[i] = 0;
		}
	}

	d += 2;

	// parse and recursively store data
	d = fd_main_load(d, d_end, m_sequences, m_nsequences);

	// cleanup and finish
	delete[] data;

	m_loaded = 1;

	return 1;
}

static char *split_line(char **data) {
	char *start = *data;

	while (*start == ' ' || *start == '\t') {
		++start;
	}

	char *p = start;
	while (*p && *p != '\n' && *p != '\r') ++p;

	if (*p) {
		*p++ = '\0';
	}
	*data = p;

	return start;
}

bool MBTL_FrameData::load_move_list(MBTL_Pack *pack, const char *filename, int bsize, int offset) {
	if (!m_loaded) {
		return 0;
	}

	char *data;
	unsigned int size;

	if (!pack->read_file(filename, &data, &size, bsize, offset)) {
		return 0;
	}

	char *line, *d;

	d = data;

	while ((line = split_line(&d)) != 0) {
		if (line[0] == '/' && line[1] == '/') {
			continue;
		}

		if (line[0] == '[') {
			break;
		}

		int move_id;
		char move_input[20];
		char move_props[20];
		char move_props_2[20];
		int move_seq;
		int move_meter;
		int move_unk1;
		int move_unk2;
		int move_unk3;

		if (sscanf(line, "%d %19s %19s %19s %d %d %d %d %d",
				&move_id,
				move_input, move_props, move_props_2,
				&move_seq, &move_meter,
				&move_unk1, &move_unk2, &move_unk3) == 9) {
			MBTL_Sequence *seq;

			seq = get_sequence(move_seq);

			if (seq) {
				std::string input = move_input;

				if (strlen(move_props) >= 8) {
					if (move_props[6] == '1' && move_props[5] == '0' && move_props[7] == '0') {
						input = "j." + input;
					}
				}

				if (seq->is_move) {
					seq->move_name = seq->move_name + " " + input;
				} else {
					seq->move_name = input;
				}
				seq->move_meter = move_meter;

				seq->is_move = 1;
			}
		}
	}

	delete[] data;

	return 1;
}

void MBTL_FrameData::free() {
	if (m_sequences) {
		for (unsigned int i = 0; i < m_nsequences; ++i) {
			MBTL_Sequence *seq = m_sequences[i];

			if (seq) {
				if (seq->data) {
					delete[] seq->data;
				}
				delete seq;
			}
		}
		delete[] m_sequences;

		m_sequences = 0;
	}
	m_nsequences = 0;

	m_loaded = 0;
}

int MBTL_FrameData::get_sequence_count() {
	if (!m_loaded) {
		return 0;
	}
	return m_nsequences;
}

MBTL_Sequence *MBTL_FrameData::get_sequence(int n) {
	if (!m_loaded) {
		return 0;
	}

	if (n < 0 || (unsigned int)n >= m_nsequences) {
		return 0;
	}

	MBTL_Sequence *seq = m_sequences[n];
	if (seq && !seq->initialized) {
		return 0;
	}

	return seq;
}

MBTL_FrameData::MBTL_FrameData() {
	m_sequences = 0;

	m_nsequences = 0;

	m_loaded = 0;
}

MBTL_FrameData::~MBTL_FrameData() {
	free();
}
