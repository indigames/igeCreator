package net.indigames.lib;

import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Resources;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public final class StorageManager {
    private final File dataDir;
    private final AssetManager assetManager;
    private final String path;
    private static final int BUFFER_SIZE = 1024;
    public static StorageManager sInstance;

    public final String getDataPath() {
        return this.dataDir.getAbsolutePath();
    }

    public final void initData() {
        try {
            this.copyFiles((String) null, this.path, this.dataDir);
        } catch (IOException ex) {

        }
    }

    public final void deleteData() throws IOException {
        this.deleteAll(this.dataDir);
    }

    private final void copyFiles(String parentPath, String filename, File toDir) throws IOException {
        String assetPath = parentPath != null ? parentPath + File.separator + filename : filename;
        if (this.isDirectory(assetPath)) {
            if (!toDir.exists()) {
                toDir.mkdirs();
            }
            for(String child :this.assetManager.list(assetPath) ) {
                this.copyFiles(assetPath, child, new File(toDir, child));
            }
        } else {
            InputStream handle = this.assetManager.open(assetPath);
            File file = new File(toDir.getParentFile(), filename);
            FileOutputStream stream = new FileOutputStream(file);
            this.copyData(handle, (OutputStream)stream);
        }
    }

    private final boolean isDirectory(String path) throws IOException {
        boolean isDirectory = false;
        try {
            if (this.assetManager.list(path).length != 0) {
                isDirectory = true;
            } else {
                this.assetManager.open(path);
            }
        } catch (FileNotFoundException var4) {
            isDirectory = true;
        }
        return isDirectory;
    }

    private final void deleteAll(File file) {
        if (file.isDirectory()) {
            for(File f : file.listFiles()) {
                this.deleteAll(f);
            }
        }
        file.delete();
    }

    private final void unzip(InputStream is, File toDir) throws IOException {
        ZipInputStream zis = null;
        try {
            zis = new ZipInputStream(is);
            ZipEntry entry = null;
            do {
                entry = zis.getNextEntry();;
                if (entry == null) {
                    break;
                }
                String entryFilePath = entry.getName().replace('\\', File.separatorChar);
                File outFile = new File(toDir, entryFilePath);
                if (entry.isDirectory()) {
                    outFile.mkdirs();
                } else {
                    this.writeData((InputStream)zis, (OutputStream)(new FileOutputStream(outFile)));
                    zis.closeEntry();
                }
            } while(true);
        } catch (IOException ex) {
        }

        try {
            if (zis != null) {
                zis.close();
            }
        } catch (IOException var12) {
        }

    }

    private final void copyData(InputStream in, OutputStream out) throws IOException {
        BufferedInputStream bis = new BufferedInputStream(in);
        try {
            this.writeData((InputStream)bis, out);
        } finally {
            try {
                bis.close();
            } catch (IOException var9) {
            }

        }
    }

    private final void writeData(InputStream is, OutputStream os) throws IOException {
        BufferedOutputStream bos = (BufferedOutputStream)null;
        try {
            bos = new BufferedOutputStream(os);
            byte[] buffer = new byte[BUFFER_SIZE];
            do {
                int len = is.read(buffer, 0, buffer.length);
                if (len <= 0) {
                    bos.flush();
                    break;
                }

                bos.write(buffer, 0, len);
            } while(true);
        } catch (IOException ex) {
        }
        try {
            if(bos != null)
                bos.close();
        } catch (IOException ex) {
        }

    }

    public StorageManager(final Context context, final String path, boolean useExternal) {
        super();
        this.path = path;
        if (useExternal) {
            this.dataDir = new File(context.getExternalFilesDir((String)null), this.path);
        } else {
            this.dataDir = context.getDir(this.path, 0);
        }
        this.assetManager = context.getResources().getAssets();
        sInstance = (StorageManager)this;
    }
}
