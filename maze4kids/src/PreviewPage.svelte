<script lang="ts">
    import { Button } from "flowbite-svelte";
    import { type BuildMazeParams } from "./lib/maze/PaperSize";
    import { Toast } from 'flowbite-svelte';
    import { CloseCircleSolid } from 'flowbite-svelte-icons';
    import { buildMaze, BuildMazeResult } from "./lib/maze/BuildMaze";
    import LeftArrowIcon from "./lib/LeftArrowIcon.svelte";
    import PrinterIcon from "./lib/PrinterIcon.svelte";

    interface Props {
      buildParams: BuildMazeParams;
    }

    const { buildParams }: Props = $props();

    let openWindowError = $state(false);

    const result = buildMaze(buildParams);
    if (result === BuildMazeResult.ErrCannotOpenWindow) {
      openWindowError = true;
    }
</script>

<div>
  <div class="flex w-full justify-between">
    <Button class="m-4 pl-8" onclick={() => history.back()}>
      <div class="me-3">
        <LeftArrowIcon />
      </div>
    </Button>
    <Button class="m-4 pl-8" onclick={() => history.back()}>
      <div class="me-3">
        <PrinterIcon />
      </div>
    </Button>
  </div>

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

  PREVIEW

</div>
