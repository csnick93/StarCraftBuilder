# -*- coding: utf-8 -*-
"""
Created on Sat Dec 17 11:45:04 2016

@author: nickv

convert tech tree into c++ code
"""

import csv

path = r"C:\Users\nickv\Documents\WS1617\AdvPT\Projekt\techtrees.csv"
outpath = r"C:\Users\nickv\Documents\WS1617\AdvPT\Projekt\techtreeCode.txt"


"""
def getLine2(produced_by):
    produced_by = produced_by.split("/")
    line2 = "vector<TypeID>{"
    for i in range(len(produced_by)-1):
        line2 += "TypeID(string(\""+produced_by[i]+"\")),"
    line2 += "TypeID(string(\""+produced_by[-1]+"\"))},"
    return line2


def getLine3(dependency):
    if dependency == "":
        return "vector<TypeID>{})},"
    dependency = dependency.split("/")
    line3 = "vector<TypeID>{"
    for i in range(len(dependency)-1):
        line3 += "TypeID(string(\""+dependency[i]+"\")),"
    line3 += "TypeID(string(\""+dependency[-1]+"\"))})},"
    return line3                        




#     0          1          2          3              4                 5                 6             7           8         9              10    
# ['#name', 'minerals', 'vespene', 'build time', 'supply cost', 'supply provided', 'start energy', 'max energy', 'race', 'produced_by', 'dependency']

def writeGameObjectMap():
    with open(outpath,"w") as f:
        current_race = "protoss"
        f.write("##############################\n")
        f.write(current_race+"\n")
        f.write("##############################\n")
        for row in dataMatrix:
            # skip description lines
            if("#" in row[0]):
                continue
            #########
            if len(row) == 11:
                name, minerals, vespene, build_time, supply_cost, supply_provided, start_energy, max_energy, race, produced_by, dependency = row
            else:
                name, minerals, vespene, build_time, supply_cost, supply_provided, start_energy, max_energy, race, produced_by= row
                dependency = ""
            if not current_race == race:
                current_race = race
                f.write("##############################\n")
                f.write(current_race+"\n")
                f.write("##############################\n")
            line1 = "{\""+name+"\",GameObjectData(\""+name+"\",Cost(Minerals(FixedPoint("+minerals+"),FixedPoint("+vespene+")),"+build_time+","+supply_cost+"), TypeID(string(\""+name+"\")),"
            line2 = getLine2(produced_by)
            line3 = getLine3(dependency)           
            f.write(line1+"\n")
            f.write(line2+"\n")
            f.write(line3+"\n")
            f.write("//\n")
"""       

"""
read in excel file data to a matrix
"""
def readInExcelFile(path):
    dataMatrix = []
    with open(path,"rb") as csvfile:
        reader = csv.reader(csvfile,delimiter=";",quotechar='|')
        for row in reader:
            row = [r.rstrip(" ").lstrip(" ") for r in row]
            dataMatrix.append(row)
    return dataMatrix

"""
Filter Terran data out of data matrix
"""
def filterTerranData(dataMatrix):
    copy = False
    terranData = []
    for row in dataMatrix:
        if "Terran" in row[0]:
            # set copy to true as soon as we have reached the terran data
            copy = True
            continue
        if copy and not "#" in row[0]:
            terranData.append(row)
    return terranData
            
"""
for gameObjectManager::create()
"""
def writeCreateFunction(terranMatrix):
    with open(outpath,"w") as f:
        for i,row in enumerate(terranMatrix):
            unit = row[0]
            if i == 0:
                f.write("if (gameObject->toString() == \""+unit.title()+"\"){\n")
                f.write("\treturn unique_ptr<GameObject>(new "+unit.title()+"(s,i));\n")
                f.write("}\n")
            else:
                f.write("else if (gameObject->toString() == \""+unit.title()+"\"){\n")
                f.write("\treturn unique_ptr<GameObject>(new "+unit.title()+"(s,i));\n")
                f.write("}\n")
        
"""
#     0          1          2          3              4                 5                 6             7           8         9              10    
# ['#name', 'minerals', 'vespene', 'build time', 'supply cost', 'supply provided', 'start energy', 'max energy', 'race', 'produced_by', 'dependency']

for the class definitions of the concrete units
"""
def writeClassConstructorDefinitions(terranMatrix):
    with open(outpath,"w") as f:
        for i, row in enumerate(terranMatrix):
            unit = row[0]
            if len(row[9]) > 0:
                produced_by = [obj.title() for obj in row[9].split("/")]
                produced_by_string = ["TypeID::getTypeID(\""+o+"\")," for o in produced_by]
                produced_by_vec = ""
                for string in produced_by_string:
                    produced_by_vec += string
                produced_by_vec = produced_by_vec.rstrip(",")
            else:
                produced_by_vec = ""
            if len(row[10]) > 0:
                dependent_on = [obj.title() for obj in row[10].split("/")]
                dependent_on_string = ["TypeID::getTypeID(\""+o+"\")," for o in dependent_on]
                dependent_on_vec = ""
                for string in dependent_on_string:
                    dependent_on_vec += string
                dependent_on_vec = dependent_on_vec.rstrip(",")
            else:
                dependent_on_vec = ""
            baseClass  = "GameObject"
            morphFrom = "nullptr"
            if unit=="scv":
                baseClass = "BaseUnit"
            if unit in ["factory_with_tech_lab","factory_with_reactor"]:
                morphFrom = "TypeID::getTypeID(\"Factory\")"
            elif unit in ["starport_with_tech_lab","starport_with_reactor"]:
                morphFrom = "TypeID::getTypeID(\"Starport\")"
            elif unit in ["barracks_with_tech_lab","barracks_with_reactor"]:
                morphFrom = "TypeID::getTypeID(\"Barracks\")"
            elif unit in ["orbital_command","planetary_fortress"]:
                morphFrom = "TypeID::getTypeID(\"Command_Center\")"
            f.write("class "+unit.title()+" : public "+baseClass + " {")
            f.write("\npublic:")
            f.write("\n\t"+unit.title()+"(Simulator& s, Inventory* i):")
            f.write("\n\t\t"+baseClass+"(s,i,Minerals(FixedPoint("+row[1]+"),FixedPoint("+row[2]+")),Supply("+str(int(row[5])-int(row[4]))+"), BuildTime("+row[3]+"),")
            f.write("\n\t\t\tvector<TypeID*>{"+produced_by_vec+"},")
            f.write("\n\t\t\tvector<TypeID*>{"+dependent_on_vec+"}, TypeID::getTypeID(\""+unit.title()+"\"), "+morphFrom+"){}")
            f.write("\n};")
            f.write("\n\n//===========================\n\n")
                
        
dataMatrix = readInExcelFile(path)

writeClassConstructorDefinitions(filterTerranData(dataMatrix))
        