
/* $Id: Main.java 6 2005-12-05 15:43:30Z kleiner $

   This file is part of java4cpp
   Copyright (C) 2005  Stefan Palme

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

package org.kapott.wrappergen.c;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Properties;


public class Main
{
    public final static boolean DEBUG=false;
    
    public static void main(String[] args)
        throws Exception
    {
        new Main().start(args);
    }
    
    private Main() 
    {
    }
    
    private void start(String[] args)
        throws Exception
    {
        List keywords=Arrays.asList(new String[] {
            "delete","union","and","or","xor","not","NULL"
        });
        List classesToDo=new ArrayList();
        
        // config-file einlesen, dabei alle value=wert-paare als 
        // properties in "config" speichern
        BufferedReader reader=new BufferedReader(new FileReader(args[0]));
        String         line;
        Properties     config=new Properties();
        while ((line=reader.readLine())!=null) {
            String[] keyvalue=line.split("=",2);
            if (keyvalue.length!=2) {
                continue;
            }
            
            String key=keyvalue[0].trim();
            String value=keyvalue[1].trim();
            
            // im value ersetzen von ${varname} durch den wert eines vorher
            // definierten properties "varname"
            for (Iterator i=config.keySet().iterator();i.hasNext();) {
                String vname=(String)i.next();
                String vvalue=config.getProperty(vname);
                value=value.replaceAll("\\$\\{"+vname+"\\}", vvalue);
            }
            
            config.setProperty(key,value);
        }
        reader.close();
        
        // config auswerten
        String  targetPath=config.getProperty("targetpath",".");
        boolean verbose=config.getProperty("verbose","").equals("true");
        
        // classfile einlesen und alle darin enthaltenen klassen zur liste
        // hinzufügen
        reader=new BufferedReader(new FileReader(config.getProperty("classfile")));
        while ((line=reader.readLine())!=null) {
            String clName=line.trim();
            if (clName.length()==0) {
                continue;
            }
            
            classesToDo.add(Class.forName(clName));
        }
        reader.close();
        
        // include-file für class-inclused erzeugen
        PrintWriter classIncludefile=new PrintWriter(new FileWriter(targetPath+File.separator+"java_classes.h"));
        classIncludefile.println("#ifndef JAVA_CLASSES_H");
        classIncludefile.println("#define JAVA_CLASSES_H");
        
        // alle zu bearbeitenden klassen durchlaufen
        for (int c=0;c<classesToDo.size();c++) {
            Class         cl=(Class)classesToDo.get(c);
            ClassAnalyzer cla=new ClassAnalyzer(cl);
            
            // daten über klassen-namen ermitteln
            String   className=cl.getName();
            String   classNameMangled=cla.getMangledName();
            String   classNameSlashed=cla.getSlashedName();
            String   classNameNamespaced=cla.getNamespacedName();
            String[] classNamespaces=cla.getNamespaces();
            String   classNamePlain=classNamespaces[classNamespaces.length-1];
            
            boolean  isBaseObject=cl.equals(Object.class);
            
            if (verbose) {
                System.out.println("generating c++-wrapper for "+className);
            }
            
            // Header-Files erzeugen
            
            // eintrag in include-file für alle klassen schreiben
            classIncludefile.println("#include <"+classNameMangled+".h>");
            
            // file open
            PrintWriter outputH=new PrintWriter(new FileWriter(targetPath+File.separator+classNameMangled+".h"));
            
            outputH.println("#ifndef "+classNameMangled+"_H");
            outputH.println("#define "+classNameMangled+"_H");
            
            // includes für alle benötigten Klassen in header-file schreiben
            // und benötigte klassen zur internen liste aller zu wrappenden 
            // klassen hinzufügen
            outputH.println("#include <jni.h>");
            outputH.println("#include <java_marker.h>");
            
            Class[] requiredClasses=cla.getAllRequiredClasses();
            Class   parent=cla.getSuperClass();
            for (int i=0;i<requiredClasses.length;i++) {
                Class         requiredClass=requiredClasses[i];
                ClassAnalyzer requiredCLA=new ClassAnalyzer(requiredClass);
                String        requiredMangled=requiredCLA.getMangledName();
                String[]      requiredNamespaces=requiredCLA.getNamespaces();
                String        requiredPlain=requiredNamespaces[requiredNamespaces.length-1];
                
                if (requiredMangled.equals(classNameMangled)) {
                    // mich selbst nicht mit in die liste aufnehmen
                    continue;
                }
                
                if (requiredClass.equals(parent)) {
                    // für basis-klasse ein include-erzeugen
                    outputH.println("#include <"+requiredMangled+".h>");
                } else {
                    // für alle "benutzten" klassen nur eine forward-deklaration erzeugen
                    outputH.println();
                    String[] namespaces=requiredCLA.getNamespaces();
                    for (int n=0;n<namespaces.length-1;n++) {
                        outputH.println("namespace "+namespaces[n]+" {");
                    }
                    outputH.println("class "+requiredPlain+";");
                    for (int n=0;n<namespaces.length-1;n++) {
                        outputH.println("}");
                    }
                }
                
                if (!classesToDo.contains(requiredClass)) {
                    classesToDo.add(requiredClass);
                }
            }
            
            // zu Object unsere eigene basis-klasse als parent hinzufügen
            if (isBaseObject) {
                outputH.println("#include <java_base_class.h>");
            }
            
            // array-klassen-definitionen einbinden
            outputH.println("class JavaByteArray;");
            outputH.println("class JavaBooleanArray;");
            outputH.println("class JavaCharArray;");
            outputH.println("class JavaIntArray;");
            outputH.println("class JavaShortArray;");
            outputH.println("class JavaLongArray;");
            outputH.println("class JavaDoubleArray;");
            outputH.println("class JavaFloatArray;");
            outputH.println("class JavaObjectArray;");
            
            // namespace open
            outputH.println();
            for (int i=0;i<classNamespaces.length-1;i++) {
                outputH.println("namespace "+classNamespaces[i]+" {");
            }
            
            // class open
            outputH.print("class "+classNamePlain);
            if (parent!=null) {
                outputH.print(" : public "+new ClassAnalyzer(parent).getNamespacedName());
            } else {
                outputH.print(" : public JavaBaseClass");
            }
            outputH.println(" {");
            outputH.println("  public:");
            
            // konstruktoren
            
            // new Klass(JavaMarker*) 
            outputH.println("    "+classNamePlain+"(JavaMarker* dummy);");
            // konstruktur, um instanz aus einem schon existierenden java-objekt zu erzeugen
            outputH.println("    "+classNamePlain+"(jobject obj);");
            
            // alle in der api definierten konstrukturen
            Constructor[] constructors=cl.getDeclaredConstructors();
            List          signatures=new ArrayList();
            for (int i=0;i<constructors.length;i++) {
                Constructor constructor=constructors[i];
                if (!Modifier.isPublic(constructor.getModifiers())) {
                    // nur public-konstruktoren
                    continue;
                }
                
                ConstructorAnalyzer cona=new ConstructorAnalyzer(constructor);
                String              signature=cona.getNativeSignature();
                
                // check auf doppelte native signaturen (weil unterschiedliche
                // java-signatures durchaus auf identische native signatures abgebildet
                // werden könnten: byte <--> char)
                if (!signatures.contains(signature)) {
                    outputH.println("    "+classNamePlain+"("+signature+"); // "+constructor.toString());
                    signatures.add(signature);
                } else {
                    if (verbose) {
                        System.out.println("skipping constructor "+classNameNamespaced+"("+signature+") because of duplicate native signature");
                    }
                }
            }
            
            outputH.println();
            
            // methode updateAllVariables
            outputH.println("    virtual void updateAllVariables(JavaMarker* dummy);");
            outputH.println("    virtual void updateAllNonFinalVariables(JavaMarker* dummy);");
            
            // alle deklarierten methoden
            Method[] methods=cl.getDeclaredMethods();
            Map      signatureMap=new Hashtable();
            for (int i=0;i<methods.length;i++) {
                Method method=methods[i];
                if (!Modifier.isPublic(method.getModifiers())) {
                    // nur public methoden
                    continue;
                }
                
                MethodAnalyzer methoda=new MethodAnalyzer(method);
                String         signature=methoda.getNativeSignature();
                
                // methodennamen, die keywords entsprechen, manglen
                String name=method.getName();
                if (keywords.contains(name)) {
                    name=name+"_";
                }
                
                // check auf doppelte native signaturen
                signatures=(List)signatureMap.get(name);
                if (signatures==null) {
                    signatures=new ArrayList();
                    signatureMap.put(name,signatures);
                }
                if (!signatures.contains(signature)) {
                    outputH.println("    virtual "+methoda.getNativeReturnType()+" "+name+"("+signature+"); // "+method.toString());
                    signatures.add(signature);
                } else {
                    if (verbose) {
                        System.out.println("skipping method "+classNameNamespaced+"."+name+"("+signature+") because of duplicate native signature");
                    }
                }
            }
            
            outputH.println();
            
            // fields
            Field[] fields=cl.getDeclaredFields();
            for (int i=0;i<fields.length;i++) {
                Field field=fields[i];
                if (!Modifier.isPublic(field.getModifiers())) {
                    continue;
                }
                
                FieldAnalyzer fielda=new FieldAnalyzer(field);
                
                // fieldnamen, die keywords entsprechen, manglen
                String name=field.getName();
                if (keywords.contains(name)) {
                    name=name+"_";
                }
                outputH.println("    "+fielda.getNativeType()+" "+name+"; // "+field.toString());
            }
            
            // class close
            outputH.println("};");
            
            // namespace close
            for (int i=0;i<classNamespaces.length-1;i++) {
                outputH.println("}");
            }
            
            outputH.println("#endif");
            // file close
            outputH.close();
            
            
            
            // C-File erzeugen
            
            // file open
            PrintWriter outputC=new PrintWriter(new FileWriter(targetPath+File.separator+classNameMangled+".cpp"));
            
            // includes in c-file schreiben
            if (DEBUG) {
                outputH.println("#include <stdio.h>;");
            }
            outputC.println("#include <string.h>");
            outputC.println("#include <jni.h>");
            outputC.println("#include <java_marker.h>");
            outputC.println("#include <java_array.h>");
            outputC.println("#include <"+classNameMangled+".h>");
            
            // includes für andere verwendete klassen
            for (int i=0;i<requiredClasses.length;i++) {
                Class  requiredClass=requiredClasses[i];
                String name=new ClassAnalyzer(requiredClass).getMangledName();
                
                if (name.equals(classNameMangled)) {
                    // mich selbst nicht mit in die liste aufnehmen
                    continue;
                }
                
                outputC.println("#include <"+name+".h>");
            }
            
            // zu Object unsere eigene basis-klasse als parent hinzufügen
            if (isBaseObject) {
                outputH.println("#include <java_base_class.h>");
            }
            
            outputC.println("extern JNIEnv     *javaEnv;");
            outputC.println("extern JavaMarker *wrapperIntern;");
            
            // namespace open
            outputC.println();
            for (int i=0;i<classNamespaces.length-1;i++) {
                outputC.println("namespace "+classNamespaces[i]+" {");
            }
            
            // konstruktoren
            
            // JavaMarker-Konstruktor 
            outputC.println();
            outputC.println(classNamePlain+"::"+classNamePlain+"(JavaMarker* dummy)");
            if (parent!=null) {
                outputC.println("  : "+new ClassAnalyzer(parent).getNamespacedName()+"(dummy)");
            } else {
                outputC.println("  : JavaBaseClass(dummy)");
            }
            outputC.println("{");
            if (DEBUG) {
                outputC.println("  printf(\"in constructor "+classNameNamespaced+"(JavaMarker*)\\n\");");
            }
            outputC.println("  updateAllVariables(wrapperIntern);");
            outputC.println("}");
            
            // jobject-Konstruktor
            outputC.println();
            outputC.println(classNamePlain+"::"+classNamePlain+"(jobject obj)");
            if (parent!=null) {
                outputC.println("  : "+new ClassAnalyzer(parent).getNamespacedName()+"(obj)");
            } else {
                outputC.println("  : JavaBaseClass(obj)");
            }
            outputC.println("{");
            if (DEBUG) {
                outputC.println("  printf(\"in constructor "+classNameNamespaced+"(jobject=%p)\\n\", obj);");
            }
            outputC.println("  updateAllVariables(wrapperIntern);");
            outputC.println("}");
            
            // alle deklarierten konstruktoren
            signatures.clear();
            for (int i=0;i<constructors.length;i++) {
                Constructor constructor=constructors[i];
                if (!Modifier.isPublic(constructor.getModifiers())) {
                    // nur public-konstruktoren
                    continue;
                }
                
                ConstructorAnalyzer cona=new ConstructorAnalyzer(constructor);
                String              signature=cona.getNativeSignature();
                
                // check auf doppelte native signaturen (weil unterschiedliche
                // java-signatures durchaus auf identische native signatures abgebildet
                // werden könnten: byte <--> char)
                if (!signatures.contains(signature)) {
                    outputC.println();
                    // signatur
                    outputC.println(classNamePlain+"::"+classNamePlain+"("+signature+")");
                    if (parent!=null) {
                        outputC.println("  : "+new ClassAnalyzer(parent).getNamespacedName()+"(wrapperIntern)");
                    } else {
                        outputC.println("  : JavaBaseClass(wrapperIntern)");
                    }
                    outputC.println("{");
                    if (DEBUG) {
                        outputC.println("  printf(\"in constructor "+classNameNamespaced+"("+signature+")\\n\");");
                    }
                    
                    // clazz holen
                    outputC.println("  jclass    cls = javaEnv->FindClass(\""+classNameSlashed+"\");");
                    outputC.println("  handleJavaException(wrapperIntern);");
                    if (DEBUG) {
                        outputC.println("  printf(\""+classNameNamespaced+"(): found cls=%p\\n\", cls);");
                    }
                    
                    // method-id für konstruktor holen
                    outputC.println("  jmethodID mid = javaEnv->GetMethodID(cls, \"<init>\", \"("+cona.getJavaSignature()+")V\");");
                    outputC.println("  handleJavaException(wrapperIntern);");
                    if (DEBUG) {
                        outputC.println("  printf(\""+classNameNamespaced+"(): found mid=%p\\n\", mid);");
                    }
                    
                    // argumente für aufruf aufbereiten
                    Class[] params=constructor.getParameterTypes();
                    
                    for (int j=0;j<params.length;j++) {
                        Class    param=params[j];
                        Datatype dt=Datatype.getInstance(param);
                        String[] code=dt.native2JavaPre("arg"+(j+1), "jarg"+(j+1));
                        for (int l=0;l<code.length;l++) {
                            if (code[l]!=null) {
                                outputC.println("  "+code[l]);
                            }
                        }
                    }
                    
                    // konstruktor aufrufen
                    outputC.print("  jobject o = javaEnv->NewObject(cls, mid");
                    for (int j=0;j<params.length;j++) {
                        outputC.print(", jarg"+(j+1));
                    }
                    outputC.println(");");
                    outputC.println("  handleJavaException(wrapperIntern);");
                    if (DEBUG) {
                        outputC.println("  printf(\""+classNameNamespaced+"(): created o=%p\\n\", o);");
                    }
                    
                    // erzeugtes objekt speichern
                    outputC.println("  setJavaObject(o);");
                    if (DEBUG) {
                        outputC.println("  printf(\""+classNameNamespaced+"(): created this.obj=%p\\n\", getJavaObject());");
                    }
                    
                    // erzeugtes objekt speichern
                    outputC.println("  javaEnv->DeleteLocalRef(o);");
                    if (DEBUG) {
                        outputC.println("  printf(\""+classNameNamespaced+"(): deleted local ref for obj=%p\\n\", o);");
                    }
                    
                    // evtl. erzeugte temporäre objekte aufräumen
                    for (int j=0;j<params.length;j++) {
                        Class    param=params[j];
                        Datatype dt=Datatype.getInstance(param);
                        String[] code=dt.native2JavaPost("arg"+(j+1), "jarg"+(j+1));
                        for (int l=0;l<code.length;l++) {
                            if (code[l]!=null) {
                                outputC.println("  "+code[l]);
                            }
                        }
                    }
                    
                    outputC.println("  updateAllVariables(wrapperIntern);");
                    outputC.println("}");
                    signatures.add(signature);
                }
            }
            
            // methoden "updateAllVariables" und "updateAllNonFinalVariables"
            // TODO: variablen müssen evtl. auch wieder "delete"d werden!
            for (int methodIdx=0;methodIdx<2;methodIdx++) {
                outputC.println();
                // signature
                String methodName=(methodIdx==0)?"updateAllVariables":"updateAllNonFinalVariables";
                outputC.println("void "+classNamePlain+"::"+methodName+"(JavaMarker* dummy)");
                outputC.println("{");
                if (DEBUG) {
                    outputC.println("  printf(\"in method "+classNameNamespaced+"::"+methodName+"()\\n\");");
                }
                
                // clazz holen
                outputC.println("  jclass cls = javaEnv->FindClass(\""+classNameSlashed+"\");");
                outputC.println("  handleJavaException(wrapperIntern);");
                if (DEBUG) {
                    outputC.println("  printf(\""+classNameNamespaced+"::"+methodName+"(): found cls=%p\\n\", cls);");
                }
                
                // für alle felder entsprechende update-methoden schreiben
                for (int i=0;i<fields.length;i++) {
                    Field field=fields[i];
                    int   modifier=field.getModifiers();
                    
                    if (!Modifier.isPublic(modifier) || 
                            (methodIdx==1 && Modifier.isFinal(modifier))) 
                    {
                        // für "updateAllNonFinalVariables" alls final-variablen überspringen
                        continue;
                    }
                    
                    FieldAnalyzer fielda=new FieldAnalyzer(field);
                    String        name=field.getName();
                    
                    // method-id holen, dabei nach static/non-static unterscheiden
                    if (Modifier.isStatic(modifier)) {
                        outputC.println("  jfieldID fid_"+name+" = javaEnv->GetStaticFieldID(cls, \""+name+"\", \""+fielda.getJavaSignature()+"\");");
                    } else {
                        outputC.println("  jfieldID fid_"+name+" = javaEnv->GetFieldID(cls, \""+name+"\", \""+fielda.getJavaSignature()+"\");");
                    }
                    outputC.println("  handleJavaException(wrapperIntern);");
                    if (DEBUG) {
                        outputC.println("  printf(\""+classNameNamespaced+"::"+methodName+"(): found fid=%p\\n\", fid_"+name+");");
                    }
                    
                    // rückgabe-variable deklarieren
                    outputC.print("  "+fielda.getJNIReturnType()+" jresult_"+name+" = ");
                    
                    // wert abholen, dabei nach static und non-static unterscheiden
                    if (Modifier.isStatic(modifier)) {
                        outputC.println("javaEnv->GetStatic"+fielda.getReturnTypeName()+"Field(cls, fid_"+name+");");
                    } else {
                        outputC.println("javaEnv->Get"+fielda.getReturnTypeName()+"Field(this->getJavaObject(), fid_"+name+");");
                    }
                    outputC.println("  handleJavaException(wrapperIntern);");
                    if (DEBUG) {
                        outputC.println("  printf(\""+classNameNamespaced+"::"+methodName+"(): value of field fetched\\n\");");
                    }
                    
                    // rückgabewert in native format konvertieren udn zurückgeben
                    String[] code=Datatype.getInstance(field.getType()).java2Native("jresult_"+name, "result_"+name);
                    for (int l=0;l<code.length;l++) {
                        if (code[l]!=null) {
                            outputC.println("  "+code[l]);
                        }
                    }
		    String mName=name;
	            if (keywords.contains(name)) {
                    	mName=name+"_";
		    }
                    outputC.println("  this->"+mName+" = result_"+name+";");
                }
                outputC.println("}");
            }
            
            // deklarierte methoden
            signatureMap.clear();
            for (int i=0;i<methods.length;i++) {
                Method method=methods[i];
                int    modifier=method.getModifiers();
                if (!Modifier.isPublic(modifier)) {
                    // nur public-methoden
                    continue;
                }
                
                MethodAnalyzer methoda=new MethodAnalyzer(method);
                String         signature=methoda.getNativeSignature();
                
                // methodennamen, die keywords entsprechen, manglen
                String name=method.getName();
                if (keywords.contains(name)) {
                    name=name+"_";
                }
                
                // check auf doppelte native signaturen (weil unterschiedliche
                // java-signatures durchaus auf identische native signatures abgebildet
                // werden könnten: byte <--> char)
                signatures=(List)signatureMap.get(name);
                if (signatures==null) {
                    signatures=new ArrayList();
                    signatureMap.put(name,signatures);
                }
                if (!signatures.contains(signature)) {
                    outputC.println();
                    // signature
                    outputC.println(methoda.getNativeReturnType()+" "+classNamePlain+"::"+name+"("+signature+")");
                    outputC.println("{");
                    if (DEBUG) {
                        outputC.println("  printf(\"in method "+classNameNamespaced+"::"+name+"("+signature+")\\n\");");
                    }
                    
                    // clazz holen
                    outputC.println("  jclass    cls = javaEnv->FindClass(\""+classNameSlashed+"\");");
                    outputC.println("  handleJavaException(wrapperIntern);");
                    if (DEBUG) {
                        outputC.println("  printf(\""+classNameNamespaced+"::"+name+"(): found cls=%p\\n\", cls);");
                    }
                    
                    // method-id holen, dabei nach static/non-static unterscheiden
                    if (Modifier.isStatic(modifier)) {
                        outputC.println("  jmethodID mid = javaEnv->GetStaticMethodID(cls, \""+name+"\", \"("+methoda.getJavaSignature()+")"+methoda.getJavaReturnType()+"\");");
                    } else {
                        outputC.println("  jmethodID mid = javaEnv->GetMethodID(cls, \""+name+"\", \"("+methoda.getJavaSignature()+")"+methoda.getJavaReturnType()+"\");");
                    }
                    outputC.println("  handleJavaException(wrapperIntern);");
                    if (DEBUG) {
                        outputC.println("  printf(\""+classNameNamespaced+"::"+name+"(): found mid=%p\\n\", mid);");
                    }
                    
                    // argumente für aufruf aufbereiten
                    Class[] params=method.getParameterTypes();
                    
                    for (int j=0;j<params.length;j++) {
                        Class    param=params[j];
                        Datatype dt=Datatype.getInstance(param);
                        String[] code=dt.native2JavaPre("arg"+(j+1), "jarg"+(j+1));
                        for (int l=0;l<code.length;l++) {
                            if (code[l]!=null) {
                                outputC.println("  "+code[l]);
                            }
                        }
                    }
                    
                    // aufruf der methode
                    
                    // bei void-methoden müssen keine rückgabedaten ausgewertet werden
                    boolean isVoid=methoda.getReturnTypeName().equals("Void");
                    
                    // rückgabe-variable deklarieren
                    outputC.print("  ");
                    if (!isVoid) {
                        outputC.print(methoda.getJNIReturnType()+" jresult=");
                    }
                    
                    // methode aufrufen, dabei nach static und non-static unterscheiden
                    if (Modifier.isStatic(modifier)) {
                        outputC.print("javaEnv->CallStatic"+methoda.getReturnTypeName()+"Method(cls, mid");
                    } else {
                        outputC.print("javaEnv->Call"+methoda.getReturnTypeName()+"Method(this->getJavaObject(), mid");
                    }
                    for (int j=0;j<params.length;j++) {
                        outputC.print(", jarg"+(j+1));
                    }
                    outputC.println(");");
                    outputC.println("  handleJavaException(wrapperIntern);");
                    if (DEBUG) {
                        outputC.println("  printf(\""+classNameNamespaced+"::"+name+"(): java method called\\n\");");
                    }
                    
                    // alle evtl. verwendeten temporären objekte wieder aufräumen
                    for (int j=0;j<params.length;j++) {
                        Class    param=params[j];
                        Datatype dt=Datatype.getInstance(param);
                        String[] code=dt.native2JavaPost("arg"+(j+1), "jarg"+(j+1));
                        for (int l=0;l<code.length;l++) {
                            if (code[l]!=null) {
                                outputC.println("  "+code[l]);
                            }
                        }
                    }
                    
                    // rückgabewert in native format konvertieren udn zurückgeben
                    if (!isVoid) {
                        String[] code=Datatype.getInstance(method.getReturnType()).java2Native("jresult", "result");
                        for (int l=0;l<code.length;l++) {
                            if (code[l]!=null) {
                                outputC.println("  "+code[l]);
                            }
                        }
                        outputC.println("  return result;");
                    }
                    
                    outputC.println("  updateAllNonFinalVariables(wrapperIntern);");
                    outputC.println("}");
                    signatures.add(signature);
                }
            }
            
            // namespace close
            for (int i=0;i<classNamespaces.length-1;i++) {
                outputC.println("}");
            }
            
            // file close
            outputC.close();
        }
        
        classIncludefile.println("#endif");
        classIncludefile.close();
    }
}
