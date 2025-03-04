<script lang="ts">
  import logoIcon from './assets/logo44.svg'
  import { Button, Card, Label, Navbar, NavBrand, Range, Select, Toast } from 'flowbite-svelte';
  import { CloseCircleSolid } from 'flowbite-svelte-icons';
  import StarIcon from './lib/StarIcon.svelte';
  import CoffeeIcon from './lib/CoffeeIcon.svelte';
  import { HexMaze } from './lib/maze/HexMaze';
  import { CreateMazeWilson } from './lib/maze/CreateMazeWilson';
  import { SvgPainter } from './lib/maze/SvgPainter';
  import { getAreaSize, inchSizeToPixelSize, type PaperSize } from './lib/maze/PaperSize';
  import { SquareMaze } from './lib/maze/SquareMaze';
  import { type GridSize } from './lib/maze/MazeGrid';

  let selectedPaperSize: PaperSize = $state('a4');
  const paperSizes = [
    { value: 'a3', name: 'A3' },
    { value: 'a4', name: 'A4' },
    { value: 'a5', name: 'A5' }
  ];

  type CellShape = 'square' | 'hexagonal';

  let selectedCellShape: CellShape = $state('square');
  const cellShapes = [
    { value: 'square', name: 'Square' },
    { value: 'hexagonal', name: 'Hexagonal' },
  ];

  let cellSize = $state(40);

  let openWindowError = $state(false);

  function buildMaze() {
    const QR_SIZE = 0.787; // ~2cm
    const QR_MARGIN = 0.196; // ~0.5cm
    const STROKE_WIDTH = 2;

    const areaSize = getAreaSize(selectedPaperSize);
    const qrSize = inchSizeToPixelSize({width: QR_SIZE + QR_MARGIN, height: QR_SIZE + QR_MARGIN});

    let maze;
    let gridSize: GridSize;
    let qrGridSize: GridSize;
    switch (selectedCellShape) {
      case 'square': {
        gridSize = SquareMaze.ComputeGridSize(areaSize.width, areaSize.height, cellSize, cellSize, STROKE_WIDTH);
        maze = new SquareMaze(gridSize.rows, gridSize.cols);
        qrGridSize = SquareMaze.ComputeGridSize(qrSize.width, qrSize.height, cellSize, cellSize, STROKE_WIDTH);
        break;
      }
      case 'hexagonal': {
        gridSize = HexMaze.ComputeGridSize(areaSize.width, areaSize.height, cellSize, STROKE_WIDTH);
        maze = new HexMaze(gridSize.rows, gridSize.cols);
        qrGridSize = HexMaze.ComputeGridSize(qrSize.width, qrSize.height, cellSize, STROKE_WIDTH);
        break;
      }
    }

    // Block out the bottom left corner for the QR code
    maze.invalidateRegion(
      {i: gridSize.rows - qrGridSize.rows - 1, j: 0},
      {i: gridSize.rows - 1, j: qrGridSize.cols + 1},
    );

    // Apply Wilson's algorithm
    const m = new MersenneTwister();
    const generator = new CreateMazeWilson(() => m.random());
    generator.createMaze(maze);
    maze.AddExits();

    const printWindow = window.open('', '', `width=${areaSize.width},height=${areaSize.height}`);
    if (printWindow === null) {
      openWindowError = true;
      return;
    }

    // Print the maze
    const svgDivNode = printWindow.document.createElement('div');
    svgDivNode.style.position = "absolute";
    const painter = new SvgPainter(svgDivNode, { strokeWidth: STROKE_WIDTH });
    maze.Draw(painter, {
      cellWidth: cellSize,
      cellHeight: cellSize,
      strokeWidth: STROKE_WIDTH,
      drawCells: false,
    });

    // Print the QR code
    const qrMargin = inchSizeToPixelSize({width: QR_MARGIN, height: QR_MARGIN});
    const qrDivNode = printWindow.document.createElement('div');
    qrDivNode.style.left = `${qrMargin.width}px`;
    qrDivNode.style.top = `${areaSize.height - qrSize.height}px`;
    qrDivNode.style.position = "relative";
    const imgNode = printWindow.document.createElement('img');
    const size = inchSizeToPixelSize({width: 0.787, height: 0.787});
    imgNode.setAttribute('width', `${size.width}px`);
    imgNode.setAttribute('height', `${size.height}px`);
    imgNode.setAttribute('src', '/images/qr-maze4kids-netli.svg');
    qrDivNode.appendChild(imgNode);

    // Add both to the document
    printWindow.document.body.setAttribute('style', 'margin: 0 0 0 0');
    printWindow.document.body.appendChild(svgDivNode);
    printWindow.document.body.appendChild(qrDivNode);
    printWindow.document.close();
  }

</script>

<main>
  <Navbar>
    <NavBrand>
      <img src="{logoIcon}" class="me-3" alt="Logo" />
      <span class="self-center whitespace-nowrap text-xl font-semibold dark:text-white">Maze4Kids</span>
    </NavBrand>
  </Navbar>

  {#if openWindowError}
    <Toast
      color="red"
      class="fixed top-5 right-5 border rounded-lg shadow-sm"
      on:close={() => { openWindowError = false; }}
      >
      <svelte:fragment slot="icon">
        <CloseCircleSolid class="w-5 h-5" />
        <span class="sr-only">Error icon</span>
      </svelte:fragment>
        Could not open preview window
    </Toast>
  {/if}

  <Card class="m-auto">
    <div class="flex flex-col space-y-6">
      <h1 class="text-3xl text-gray-900 dark:text-white">Design your maze</h1>

      <Label class="space-y-2 text-gray-500">
        <span>Paper Size</span>
        <Select class="mt-2 my-selector" items={paperSizes} bind:value={selectedPaperSize} />
      </Label>
      <Label class="space-y-2 text-gray-500">
        <span>Cell Shape</span>
        <Select class="mt-2 my-selector" items={cellShapes} bind:value={selectedCellShape} />
      </Label>
      <Label class="space-y-2 text-gray-500">
        <div class="flex items-start">
          <span>Cell Size</span>
          <span class="ms-auto">{cellSize}</span>
        </div>
        <Range size="lg" min="20" max="100" step="5" bind:value={cellSize} />
      </Label>

      <Button class="w-full" onclick={buildMaze}>
        Build Maze
      </Button>

      <hr/>

      <div class="flex justify-around">
        <a href="{import.meta.env.VITE_GOOGLE_FORMS_URL}">
          <Button outline class="text-gray-500" color="dark" size="md">
            <div class="me-3">
              <StarIcon />
            </div>
            <span>Rate</span>
          </Button>
        </a>
        <a href="{import.meta.env.VITE_BUY_ME_COFFEE_URL}">
          <Button outline class="text-gray-500" color="dark" size="md">
            <div class="me-3">
              <CoffeeIcon />
            </div>
            <span>Donate</span>
          </Button>
        </a>
      </div>
    </div>
  </Card>
</main>
