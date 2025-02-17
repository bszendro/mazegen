<script lang="ts">
  import logoIcon from './assets/logo44.svg'
  import { Button, Card, Label, Navbar, NavBrand, Range, Select, Toast } from 'flowbite-svelte';
  import { CloseCircleSolid } from 'flowbite-svelte-icons';
  import StarIcon from './lib/StarIcon.svelte';
  import CoffeeIcon from './lib/CoffeeIcon.svelte';
  import { HexMaze } from './lib/maze/HexMaze';
  import { CreateMazeWilson } from './lib/maze/CreateMazeWilson';
  import { SvgPainter } from './lib/maze/SvgPainter';
  import { getAreaSize } from './lib/maze/PaperSize';
  import { SquareMaze } from './lib/maze/SquareMaze';

  let selectedPaperSize = $state('a4');
  const paperSizes = [
    { value: 'a3', name: 'A3' },
    { value: 'a4', name: 'A4' },
    { value: 'a5', name: 'A5' }
  ];

  let selectedCellShape = $state('square');
  const cellShapes = [
    { value: 'square', name: 'Square' },
    { value: 'hexagonal', name: 'Hexagonal' },
  ];

  let cellSize = $state(40);

  let openWindowError = $state(false);

  function buildMaze() {
    const areaSize = getAreaSize(selectedPaperSize);
    const strokeWidth = 2;
    let maze;
    switch (selectedCellShape) {
      case 'square': {
        const gridSize = SquareMaze.ComputeGridSize(areaSize.width, areaSize.height, cellSize, cellSize, strokeWidth);
        maze = new SquareMaze(gridSize.rows, gridSize.cols);
        break;
      }
      case 'hexagonal': {
        const gridSize = HexMaze.ComputeGridSize(areaSize.width, areaSize.height, cellSize, strokeWidth);
        maze = new HexMaze(gridSize.rows, gridSize.cols);
        break;
      }
    }

    const m = new MersenneTwister();
    const generator = new CreateMazeWilson(() => m.random());
    generator.createMaze(maze!);
    maze!.AddExits();

    const printWindow = window.open('', '', `width=${areaSize.width},height=${areaSize.height}`);
    if (printWindow === null) {
      openWindowError = true;
      return;
    }

    const parentNode = printWindow.document.createElement('div');
    const painter = new SvgPainter(parentNode, { strokeWidth });
    maze!.Draw(painter, {cellWidth: cellSize, cellHeight: cellSize, strokeWidth});
    printWindow.document.body.setAttribute('style', 'margin: 0 0 0 0');
    printWindow.document.body.appendChild(parentNode);
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
        <Button outline class="text-gray-500" color="dark" size="md">
          <div class="me-3">
            <StarIcon />
          </div>
          <span>Rate</span>
        </Button>
        <Button outline class="text-gray-500" color="dark" size="md">
          <div class="me-3">
            <CoffeeIcon />
          </div>
          <span>Donate</span>
        </Button>
      </div>
    </div>
  </Card>
</main>
