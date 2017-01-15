﻿#if UNITY_EDITOR
using UnityEngine;
using UnityEditor;


public class usdiPackaging
{
    [MenuItem("Assets/Make MeshSync.unitypackage")]
    public static void MakePackage()
    {
        string[] files = new string[]
        {
"Assets/UTJ",
        };
        AssetDatabase.ExportPackage(files, "MeshSync.unitypackage", ExportPackageOptions.Recurse);
    }

}
#endif // UNITY_EDITOR