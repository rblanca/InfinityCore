/*
 * Copyright (C) 2013-2015 InfinityCore <http://www.noffearrdeathproject.net/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Instance_Scarlet_Monastery
SD%Complete: 50
SDComment:
SDCategory: Scarlet Monastery
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "scarlet_monastery.h"
#include "InstanceScript.h"

enum Entry
{
    ENTRY_PUMPKIN_SHRINE    = 186267,
    ENTRY_HORSEMAN          = 23682,
    ENTRY_HEAD              = 23775,
    ENTRY_PUMPKIN           = 23694
};

DoorData const doorData[] =
{
    {GOB_KEL_DOOR, NPC_KELESTRES, DOOR_TYPE_ROOM,  BOUNDARY_NONE}, // Kelestres Door
    {GOB_ASM_DOOR, NPC_ASMODINA, DOOR_TYPE_ROOM,  BOUNDARY_NONE}, // Asmodina Door
};

#define MAX_ENCOUNTER 2

class instance_scarlet_monastery : public InstanceMapScript
{
public:
    instance_scarlet_monastery() : InstanceMapScript("instance_scarlet_monastery", 189) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_scarlet_monastery_InstanceMapScript(map);
    }

    struct instance_scarlet_monastery_InstanceMapScript : public InstanceScript
    {
        instance_scarlet_monastery_InstanceMapScript(Map* map) : InstanceScript(map) { }

        uint64 PumpkinShrineGUID;
        uint64 HorsemanGUID;
        uint64 HeadGUID;
        std::set<uint64> HorsemanAdds;

        uint64 MograineGUID;
        uint64 WhitemaneGUID;
        uint64 VorrelGUID;
        uint64 DoorHighInquisitorGUID;

        uint64 _KelDoor;
        uint64 _AsmDoor;

        uint32 encounter[MAX_ENCOUNTER];

        void Initialize()
        {
            memset(&encounter, 0, sizeof(encounter));

            PumpkinShrineGUID  = 0;
            HorsemanGUID = 0;
            HeadGUID = 0;
            HorsemanAdds.clear();

            MograineGUID = 0;
            WhitemaneGUID = 0;
            VorrelGUID = 0;
            DoorHighInquisitorGUID = 0;

            _KelDoor = 0;
            _AsmDoor = 0;
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
            case ENTRY_PUMPKIN_SHRINE:
                 PumpkinShrineGUID = go->GetGUID();
                 break;
            case 104600:
                 DoorHighInquisitorGUID = go->GetGUID();
                 break;
            case GOB_KEL_DOOR:
                 _KelDoor = go->GetGUID();
                 AddDoor(go, true);
                 break;
            case GOB_ASM_DOOR:
                 _AsmDoor = go->GetGUID();
                 AddDoor(go, true);
                 break;
            }
        }

        void OnGameObjectRemove(GameObject* go)
        {
            switch(go->GetEntry())
            {
                case GOB_KEL_DOOR:
                    AddDoor(go, false);
                    break;
                case GOB_ASM_DOOR:
                    AddDoor(go, false);
                    break;
            }
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case ENTRY_HORSEMAN:    HorsemanGUID = creature->GetGUID(); break;
                case ENTRY_HEAD:        HeadGUID = creature->GetGUID(); break;
                case ENTRY_PUMPKIN:     HorsemanAdds.insert(creature->GetGUID());break;
                case 3976: MograineGUID = creature->GetGUID(); break;
                case 3977: WhitemaneGUID = creature->GetGUID(); break;
                case 3981: VorrelGUID = creature->GetGUID(); break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
            case TYPE_MOGRAINE_AND_WHITE_EVENT:
                if (data == IN_PROGRESS)
                    DoUseDoorOrButton(DoorHighInquisitorGUID);
                if (data == FAIL)
                    DoUseDoorOrButton(DoorHighInquisitorGUID);

                encounter[0] = data;
                break;
            case GAMEOBJECT_PUMPKIN_SHRINE:
                HandleGameObject(PumpkinShrineGUID, false);
                break;
            case DATA_HORSEMAN_EVENT:
                encounter[1] = data;
                if (data == DONE)
                {
                    for (std::set<uint64>::const_iterator itr = HorsemanAdds.begin(); itr != HorsemanAdds.end(); ++itr)
                    {
                        Creature* add = instance->GetCreature(*itr);
                        if (add && add->isAlive())
                            add->Kill(add);
                    }
                    HorsemanAdds.clear();
                    HandleGameObject(PumpkinShrineGUID, false);
                }
                break;
            }
        }

        uint64 GetData64(uint32 type) const
        {
            switch (type)
            {
                //case GAMEOBJECT_PUMPKIN_SHRINE:   return PumpkinShrineGUID;
                //case DATA_HORSEMAN:               return HorsemanGUID;
                //case DATA_HEAD:                   return HeadGUID;
                case DATA_MOGRAINE:             return MograineGUID;
                case DATA_WHITEMANE:            return WhitemaneGUID;
                case DATA_VORREL:               return VorrelGUID;
                case DATA_DOOR_WHITEMANE:       return DoorHighInquisitorGUID;
                case GOB_KEL_DOOR:
                    return _KelDoor;
                case GOB_ASM_DOOR:
                    return _AsmDoor;
            }
            return 0;
        }

        uint32 GetData(uint32 type) const
        {
            if (type == TYPE_MOGRAINE_AND_WHITE_EVENT)
                return encounter[0];
            if (type == DATA_HORSEMAN_EVENT)
                return encounter[1];
            return 0;
        }
    };
};

void AddSC_instance_scarlet_monastery()
{
    new instance_scarlet_monastery();
}
