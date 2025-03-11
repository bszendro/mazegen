<script lang="ts">
  import { Button, Card, Label, Range, Select } from 'flowbite-svelte';
  import StarIcon from './lib/StarIcon.svelte';
  import CoffeeIcon from './lib/CoffeeIcon.svelte';
  import { type CellShape, type PaperSize } from './lib/maze/PaperSize';

  interface Props {
    onBuild: (paperSize: PaperSize, cellShape: CellShape, cellSize: number) => void;
  }

  const { onBuild }: Props = $props();

  let selectedPaperSize: PaperSize = $state('a4');
  const paperSizes = [
    { value: 'a3', name: 'A3' },
    { value: 'a4', name: 'A4' },
    { value: 'a5', name: 'A5' }
  ];

  let selectedCellShape: CellShape = $state('square');
  const cellShapes = [
    { value: 'square', name: 'Square' },
    { value: 'hexagonal', name: 'Hexagonal' },
  ];

  let cellSize = $state(40);
</script>

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

    <Button class="w-full" onclick={() => onBuild(selectedPaperSize, selectedCellShape, cellSize)}>
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
