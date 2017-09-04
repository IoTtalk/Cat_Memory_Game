import DAN

ServerIP='140.113.199.199' #P=None:AutoSearch, or ='IP':Connect to this IP

def profile_init():
    DAN.profile['dm_name']='Cat_Memory_Game'
    DAN.profile['d_name']=DAN.get_mac_addr()[-4:]

def odf():  # int only
    return [
      ('Ears_O'      , 0, 'Ears_O'),
      ('Face_O'      , 0, 'Face_O'),
      ('Hands_O'     , 0, 'Hands_O'),
      ('Belly_O'     , 0, 'Belly_O'),
      ('Feet_O'      , 0, 'Feet_O'),
      ('SystemMsg_O', 0, 'SystemMsg_O'),
    ]

def idf():
    return [
      ('Ears_I', int),
      ('Face_I', int),
      ('Hands_I', int),
      ('Belly_I', int),
      ('Feet_I', int),
      ('SystemMsg_I',str),
    ]
