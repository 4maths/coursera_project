import express from "express";
import cors from "cors";
import { S3Client, PutObjectCommand } from "@aws-sdk/client-s3";
import { getSignedUrl } from "@aws-sdk/s3-request-presigner";

const app = express();
app.use(cors());
app.use(express.json());

const REGION = process.env.AWS_REGION;
const BUCKET = process.env.S3_BUCKET;

if (!REGION || !BUCKET) {
  console.error("Missing AWS_REGION or S3_BUCKET env var");
  process.exit(1);
}

const s3 = new S3Client({ region: REGION });

app.post("/videos/presign", async (req, res) => {
  try {
    const { filename, content_type } = req.body;
    if (!filename || !content_type)
      return res.status(400).send("missing fields");

    if (
      filename.includes("..") ||
      filename.includes("/") ||
      filename.includes("\\")
    ) {
      return res.status(400).send("invalid filename");
    }

    const object_key = `videos/${Date.now()}_${filename}`;

    const cmd = new PutObjectCommand({
      Bucket: BUCKET,
      Key: object_key,
      ContentType: content_type,
    });

    const upload_url = await getSignedUrl(s3, cmd, { expiresIn: 900 });

    // Sửa trong presign.js
    const public_url = `http://${BUCKET}.s3.${REGION}.amazonaws.com/${object_key}`;

    res.json({ upload_url, object_key, public_url });
  } catch (e) {
    console.error(e);
    res.status(500).send("presign error");
  }
});

app.listen(3001, () =>
  console.log("Presign service running on http://localhost:3001")
);
