<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>3D Gesture Controlled Particle System</title>
  <style>
    body { margin: 0; overflow: hidden; background: black; }
    canvas { display: block; }
  </style>

  <!-- THREE.JS -->
  <script src="https://cdn.jsdelivr.net/npm/three@0.160.0/build/three.min.js"></script>

  <!-- MEDIAPIPE -->
  <script src="https://cdn.jsdelivr.net/npm/@mediapipe/hands/hands.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/@mediapipe/camera_utils/camera_utils.js"></script>
</head>

<body>

<video id="video" autoplay playsinline style="display:none;"></video>

<script>
let scene, camera, renderer, particles;
let particleGeometry, particleMaterial;
let currentShape = "sphere";
let expansion = 1;

init();
animate();
initHandTracking();

function init() {
  scene = new THREE.Scene();

  camera = new THREE.PerspectiveCamera(75, window.innerWidth/window.innerHeight, 0.1, 1000);
  camera.position.z = 6;

  renderer = new THREE.WebGLRenderer({ antialias: true });
  renderer.setSize(window.innerWidth, window.innerHeight);
  document.body.appendChild(renderer.domElement);

  createParticles();
}

function createParticles(shape="sphere") {
  if (particles) scene.remove(particles);

  const count = 3000;
  const positions = [];

  for (let i = 0; i < count; i++) {
    let x, y, z;

    if (shape === "heart") {
      const t = Math.random() * Math.PI * 2;
      x = 16 * Math.pow(Math.sin(t), 3);
      y = 13 * Math.cos(t) - 5 * Math.cos(2*t) - 2 * Math.cos(3*t) - Math.cos(4*t);
      z = (Math.random() - 0.5) * 10;
    }
    else if (shape === "saturn") {
      const angle = Math.random() * Math.PI * 2;
      const radius = 5 + Math.random();
      x = Math.cos(angle) * radius;
      y = (Math.random() - 0.5) * 0.5;
      z = Math.sin(angle) * radius;
    }
    else {
      x = (Math.random() - 0.5) * 10;
      y = (Math.random() - 0.5) * 10;
      z = (Math.random() - 0.5) * 10;
    }

    positions.push(x, y, z);
  }

  particleGeometry = new THREE.BufferGeometry();
  particleGeometry.setAttribute(
    'position',
    new THREE.Float32BufferAttribute(positions, 3)
  );

  particleMaterial = new THREE.PointsMaterial({
    size: 0.05,
    color: new THREE.Color(Math.random(), Math.random(), Math.random())
  });

  particles = new THREE.Points(particleGeometry, particleMaterial);
  scene.add(particles);
}

function animate() {
  requestAnimationFrame(animate);
  particles.rotation.y += 0.002;
  particles.scale.set(expansion, expansion, expansion);
  renderer.render(scene, camera);
}

// ---------- HAND TRACKING ----------
function initHandTracking() {
  const videoElement = document.getElementById("video");

  const hands = new Hands({
    locateFile: file =>
      `https://cdn.jsdelivr.net/npm/@mediapipe/hands/${file}`
  });

  hands.setOptions({
    maxNumHands: 1,
    modelComplexity: 1,
    minDetectionConfidence: 0.7,
    minTrackingConfidence: 0.7
  });

  hands.onResults(results => {
    if (!results.multiHandLandmarks) return;

    const hand = results.multiHandLandmarks[0];
    const thumb = hand[4];
    const index = hand[8];

    const distance = Math.hypot(
      thumb.x - index.x,
      thumb.y - index.y
    );

    expansion = THREE.MathUtils.clamp(distance * 6, 0.5, 3);

    // Gesture-based shape switching
    if (distance < 0.03) createParticles("heart");
    else if (distance > 0.15) createParticles("saturn");
  });

  const cameraFeed = new Camera(videoElement, {
    onFrame: async () => {
      await hands.send({ image: videoElement });
    },
    width: 640,
    height: 480
  });

  cameraFeed.start();
}
</script>

</body>
</html>